// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

// Loggers registry of unique name->logger pointer
// An attempt to create a logger with an already existing name will result with spdlog_ex exception.
// If user requests a non existing logger, nullptr will be returned
// This class is thread safe

#include <spdlog/common.h>
#include <spdlog/cfg/log_levels.h>

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>

namespace spdlog {
class logger;

namespace details {
class thread_pool;

class SPDLOG_API registry
{
public:
    registry(const registry &) = delete;
    registry &operator=(const registry &) = delete;

    void register_logger(std::shared_ptr<logger> new_logger);
    void initialize_logger(std::shared_ptr<logger> new_logger);
    std::shared_ptr<logger> get(const std::string &logger_name);
    std::shared_ptr<logger> default_logger();

    // Return raw ptr to the default logger.
    // To be used directly by the spdlog default api (e.g. spdlog::info)
    // This make the default API faster, but cannot be used concurrently with set_default_logger().
    // e.g do not call set_default_logger() from one thread while calling spdlog::info() from another.
    logger *get_default_raw();

    // set default logger.
    // default logger is stored in default_logger_ (for faster retrieval) and in the loggers_ map.
    void set_default_logger(std::shared_ptr<logger> new_default_logger);

    void set_tp(std::shared_ptr<thread_pool> tp);

    std::shared_ptr<thread_pool> get_tp();

    // Set global formatter. Each sink in each logger will get a clone of this object
    void set_formatter(std::unique_ptr<formatter> formatter);

    void enable_backtrace(size_t n_messages);

    void disable_backtrace();

    void set_level(level::level_enum log_level);

    void flush_on(level::level_enum log_level);

    void flush_every(std::chrono::seconds interval);

    void set_error_handler(void (*handler)(const std::string &msg));

    void apply_all(const std::function<void(const std::shared_ptr<logger>)> &fun);

    void flush_all();

    void drop(const std::string &logger_name);

    void drop_all();

    // clean all resources and threads started by the registry
    void shutdown();

#ifdef CEP_SPDLOG_MODIFIED
#ifdef CEP_SPDLOG_USE_MUTEX
    cep::Mutex &tp_mutex();
#endif
#else
    std::lock_guard<std::mutex> lock{mutex_};
    std::recursive_mutex &tp_mutex();
#endif

    void set_automatic_registration(bool automatic_registration);

    void update_levels(cfg::log_levels levels);

    static registry &instance();

private:
    registry();
    ~registry();

    void throw_if_exists_(const std::string &logger_name);
    void register_logger_(std::shared_ptr<logger> new_logger);
#ifdef CEP_SPDLOG_MODIFIED
#ifdef CEP_SPDLOG_USE_MUTEX
    cep::Mutex logger_map_mutex_;
    cep::Mutex flusher_mutex_;
    cep::Mutex tp_mutex_;
#endif
#else
    std::mutex logger_map_mutex_, flusher_mutex_;
    std::recursive_mutex tp_mutex_;
#endif

    std::unordered_map<std::string, std::shared_ptr<logger>> loggers_;
    cfg::log_levels levels_;
    std::unique_ptr<formatter> formatter_;
    level::level_enum flush_level_ = level::off;
    void (*err_handler_)(const std::string &msg);
    std::shared_ptr<thread_pool> tp_;
#ifndef CEP_SPDLOG_MODIFIED
    std::unique_ptr<periodic_worker> periodic_flusher_;
#endif
    std::shared_ptr<logger> default_logger_;
    bool automatic_registration_ = true;
    size_t backtrace_n_messages_ = 0;
};

} // namespace details
} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
#include "registry-inl.h"
#endif
