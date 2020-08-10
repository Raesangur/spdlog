// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#ifndef SPDLOG_HEADER_ONLY
#include <spdlog/sinks/stdout_sinks.h>
#endif

#include <spdlog/details/console_globals.h>
#include <spdlog/pattern_formatter.h>
#include <memory>

namespace spdlog {

namespace sinks {

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE stdout_sink_base
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::stdout_sink_base(FILE *file)
    :
#ifndef CEP_SPDLOG_MODIFIED
    mutex_(ConsoleMutex::mutex())
    ,
#endif
    file_(file)
    , formatter_(details::make_unique<spdlog::pattern_formatter>())
{}

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE void stdout_sink_base
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::log(const details::log_msg &msg)
{
#ifdef CEP_SPDLOG_MODIFIED
#ifdef CEP_SPDLOG_USE_MUTEX
    cep::Lock_Guard lock{mutex_};
#endif
#else
    std::lock_guard<std::mutex> lock(mutex_t);
#endif
    memory_buf_t formatted;
    formatter_->format(msg, formatted);

#ifdef CEP_SPDLOG_MODIFIED
    if (custom_log_handler_)
    {
        custom_log_handler_(formatted);
    }
    else
#endif
    {
        fwrite(formatted.data(), sizeof(char), formatted.size(), file_);
        fflush(file_); // flush every line to terminal
    }
}

#ifdef CEP_SPDLOG_MODIFIED
SPDLOG_INLINE void stdout_sink_base::set_custom_logger(log_handler custom_log_handler)
{
    custom_log_handler_ = std::move(custom_log_handler);
}
#endif

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE void stdout_sink_base
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::flush()
{
#ifdef CEP_SPDLOG_MODIFIED
#ifdef CEP_SPDLOG_USE_MUTEX
    cep::Lock_Guard lock{mutex_};
#endif
#else
    std::lock_guard<std::mutex> lock(mutex_t);
#endif
    fflush(file_);
}

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE void stdout_sink_base
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::set_pattern(const std::string &pattern)
{
#ifdef CEP_SPDLOG_MODIFIED
#ifdef CEP_SPDLOG_USE_MUTEX
    cep::Lock_Guard lock{mutex_};
#endif
#else
    std::lock_guard<std::mutex> lock(mutex_t);
#endif
    formatter_ = std::unique_ptr<spdlog::formatter>(new pattern_formatter(pattern));
}

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE void stdout_sink_base
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter)
{
#ifdef CEP_SPDLOG_MODIFIED
#ifdef CEP_SPDLOG_USE_MUTEX
    cep::Lock_Guard lock{mutex_};
#endif
#else
    std::lock_guard<std::mutex> lock(mutex_t);
#endif
    formatter_ = std::move(sink_formatter);
}

// stdout sink
#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE stdout_sink
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::stdout_sink()
    : stdout_sink_base
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    (stdout)
{}

// stderr sink
#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE stderr_sink
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::stderr_sink()
    : stdout_sink_base
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    (stderr)
{}

} // namespace sinks

// factory methods
template<typename Factory>
SPDLOG_INLINE std::shared_ptr<logger> stdout_logger_mt(const std::string &logger_name)
{
    return Factory::template create<sinks::stdout_sink_mt>(logger_name);
}

template<typename Factory>
SPDLOG_INLINE std::shared_ptr<logger> stdout_logger_st(const std::string &logger_name)
{
    return Factory::template create<sinks::stdout_sink_st>(logger_name);
}

template<typename Factory>
SPDLOG_INLINE std::shared_ptr<logger> stderr_logger_mt(const std::string &logger_name)
{
    return Factory::template create<sinks::stderr_sink_mt>(logger_name);
}

template<typename Factory>
SPDLOG_INLINE std::shared_ptr<logger> stderr_logger_st(const std::string &logger_name)
{
    return Factory::template create<sinks::stderr_sink_st>(logger_name);
}
} // namespace spdlog
