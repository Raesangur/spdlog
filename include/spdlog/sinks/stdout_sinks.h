// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <spdlog/details/console_globals.h>
#include <spdlog/details/synchronous_factory.h>
#include <spdlog/sinks/sink.h>
#include <cstdio>

namespace spdlog {

namespace sinks {

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
class stdout_sink_base : public sink
{
public:
#ifdef CEP_SPDLOG_MODIFIED
#ifdef CEP_SPDLOG_USE_MUTEX
    using mutex_t = cep::Mutex;
#endif
#else
    using mutex_t = typename ConsoleMutex::mutex_t;
#endif

    explicit stdout_sink_base(FILE *file);
    ~stdout_sink_base() override = default;

    stdout_sink_base(const stdout_sink_base &other) = delete;
    stdout_sink_base(stdout_sink_base &&other) = delete;

    stdout_sink_base &operator=(const stdout_sink_base &other) = delete;
    stdout_sink_base &operator=(stdout_sink_base &&other) = delete;

    void log(const details::log_msg &msg) override;
    void flush() override;
    void set_pattern(const std::string &pattern) override;

    void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override;

#ifdef CEP_SPDLOG_MODIFIED
    void set_custom_logger(log_handler custom_log_handler);
#endif

protected:
#ifdef CEP_SPDLOG_MODIFIED
#ifdef CEP_SPDLOG_USE_MUTEX
    mutex_t mutex_;
#endif
#else
    mutex_t &mutex_;
#endif
#ifdef CEP_SPDLOG_MODIFIED
    log_handler custom_log_handler_;
#endif
    FILE *file_;
    std::unique_ptr<spdlog::formatter> formatter_;
};

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
class stdout_sink : public stdout_sink_base
#ifndef CEP_SPDLOG_MODIFIED
                    <ConsoleMutex>
#endif
{
public:
    stdout_sink();
};

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
class stderr_sink : public stdout_sink_base
#ifndef CEP_SPDLOG_MODIFIED
                    <ConsoleMutex>
#endif
{
public:
    stderr_sink();
};

using stdout_sink_mt = stdout_sink
#ifndef CEP_SPDLOG_MODIFIED
    <details::console_mutex>;
#else
    ;
#endif
using stdout_sink_st = stdout_sink
#ifndef CEP_SPDLOG_MODIFIED
    <details::console_nullmutex>;
#else
    ;
#endif

using stderr_sink_mt = stderr_sink
#ifndef CEP_SPDLOG_MODIFIED
    <details::console_mutex>;
#else
    ;
#endif
using stderr_sink_st = stderr_sink
#ifndef CEP_SPDLOG_MODIFIED
    <details::console_nullmutex>;
#else
    ;
#endif

} // namespace sinks

// factory methods
template<typename Factory = spdlog::synchronous_factory>
std::shared_ptr<logger> stdout_logger_mt(const std::string &logger_name);

template<typename Factory = spdlog::synchronous_factory>
std::shared_ptr<logger> stdout_logger_st(const std::string &logger_name);

template<typename Factory = spdlog::synchronous_factory>
std::shared_ptr<logger> stderr_logger_mt(const std::string &logger_name);

template<typename Factory = spdlog::synchronous_factory>
std::shared_ptr<logger> stderr_logger_st(const std::string &logger_name);

} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
#include "stdout_sinks-inl.h"
#endif
