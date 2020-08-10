// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#ifndef SPDLOG_HEADER_ONLY
#include <spdlog/sinks/ansicolor_sink.h>
#endif

#include <spdlog/pattern_formatter.h>
#include <spdlog/details/os.h>

namespace spdlog {
namespace sinks {

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE ansicolor_sink
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::ansicolor_sink(FILE *target_file, color_mode mode)
    : target_file_(target_file)
#ifndef CEP_SPDLOG_MODIFIED
    , mutex_(ConsoleMutex::mutex())
#endif
    , formatter_(details::make_unique<spdlog::pattern_formatter>())

{
    set_color_mode(mode);
    colors_[level::trace] = to_string_(white);
    colors_[level::debug] = to_string_(cyan);
    colors_[level::info] = to_string_(green);
    colors_[level::warn] = to_string_(yellow_bold);
    colors_[level::err] = to_string_(red_bold);
    colors_[level::critical] = to_string_(bold_on_red);
    colors_[level::off] = to_string_(reset);
}

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE void ansicolor_sink
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::set_color(level::level_enum color_level, string_view_t color)
{
#ifndef CEP_SPDLOG_MODIFIED
    std::lock_guard<mutex_t> lock(mutex_);
#endif
    colors_[color_level] = to_string_(color);
}

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE void ansicolor_sink
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::log(const details::log_msg &msg)
{
// Wrap the originally formatted message in color codes.
// If color is not supported in the terminal, log as is instead.
#ifdef CEP_SPDLOG_MODIFIED
#ifdef CEP_SPDLOG_USE_MUTEX
    cep::Lock_Guard lock{mutex_};
#endif
#else
    std::lock_guard<std::mutex> lock(mutex_t);
#endif
    msg.color_range_start = 0;
    msg.color_range_end = 0;
    memory_buf_t formatted;
    formatter_->format(msg, formatted);
    if (should_do_colors_ && msg.color_range_end > msg.color_range_start)
    {
        // before color range
        print_range_(formatted, 0, msg.color_range_start);
        // in color range
        print_ccode_(colors_[msg.level]);
        print_range_(formatted, msg.color_range_start, msg.color_range_end);
        print_ccode_(reset);
        // after color range
        print_range_(formatted, msg.color_range_end, formatted.size());
    }
    else // no color
    {
        print_range_(formatted, 0, formatted.size());
    }
    fflush(target_file_);
}

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE void ansicolor_sink
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
    fflush(target_file_);
}

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE void ansicolor_sink
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
SPDLOG_INLINE void ansicolor_sink
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

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE bool ansicolor_sink
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::should_color()
{
    return should_do_colors_;
}

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE void ansicolor_sink
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::set_color_mode(color_mode mode)
{
    switch (mode)
    {
    case color_mode::always:
        should_do_colors_ = true;
        return;
    case color_mode::automatic:
        should_do_colors_ = details::os::in_terminal(target_file_) && details::os::is_color_terminal();
        return;
    case color_mode::never:
        should_do_colors_ = false;
        return;
    }
}

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE void ansicolor_sink
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::print_ccode_(const string_view_t &color_code)
{
    fwrite(color_code.data(), sizeof(char), color_code.size(), target_file_);
}

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE void ansicolor_sink
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::print_range_(const memory_buf_t &formatted, size_t start, size_t end)
{
    fwrite(formatted.data() + start, sizeof(char), end - start, target_file_);
}

#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE std::string ansicolor_sink
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::to_string_(const string_view_t &sv)
{
    return std::string(sv.data(), sv.size());
}

// ansicolor_stdout_sink
#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE ansicolor_stdout_sink
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::ansicolor_stdout_sink(color_mode mode)
    : ansicolor_sink
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    (stdout, mode)
{}

// ansicolor_stderr_sink
#ifndef CEP_SPDLOG_MODIFIED
template<typename ConsoleMutex>
#endif
SPDLOG_INLINE ansicolor_stderr_sink
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    ::ansicolor_stderr_sink(color_mode mode)
    : ansicolor_sink
#ifndef CEP_SPDLOG_MODIFIED
    <ConsoleMutex>
#endif
    (stderr, mode)
{}

} // namespace sinks
} // namespace spdlog
