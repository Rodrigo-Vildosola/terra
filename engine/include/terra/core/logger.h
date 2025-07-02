#pragma once

#include <memory>
#include <webgpu/webgpu_cpp_print.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace terra {

class logger {
public:
    static void init();

    inline static std::shared_ptr<spdlog::logger>& get_core_logger() { return core_logger; }
    inline static std::shared_ptr<spdlog::logger>& get_client_logger() { return client_logger; }
    inline static std::shared_ptr<spdlog::logger>& get_only_file_logger() { return only_file_logger; }

private:
    static std::shared_ptr<spdlog::logger> core_logger;
    static std::shared_ptr<spdlog::logger> client_logger;
    static std::shared_ptr<spdlog::logger> only_file_logger;

};

} // namespace terra

// Core macros
#define TR_CORE_TRACE(...)    ::terra::logger::get_core_logger()->trace(__VA_ARGS__)
#define TR_CORE_DEBUG(...)    ::terra::logger::get_core_logger()->debug(__VA_ARGS__)
#define TR_CORE_INFO(...)     ::terra::logger::get_core_logger()->info(__VA_ARGS__)
#define TR_CORE_WARN(...)     ::terra::logger::get_core_logger()->warn(__VA_ARGS__)
#define TR_CORE_ERROR(...)    ::terra::logger::get_core_logger()->error(__VA_ARGS__)
#define TR_CORE_CRITICAL(...) ::terra::logger::get_core_logger()->critical(__VA_ARGS__)

// CLient macros
#define TR_TRACE(...)     ::terra::logger::get_client_logger()->trace(__VA_ARGS__)
#define TR_DEBUG(...)     ::terra::logger::get_client_logger()->debug(__VA_ARGS__)
#define TR_INFO(...)      ::terra::logger::get_client_logger()->info(__VA_ARGS__)
#define TR_WARN(...)      ::terra::logger::get_client_logger()->warn(__VA_ARGS__)
#define TR_ERROR(...)     ::terra::logger::get_client_logger()->error(__VA_ARGS__)
#define TR_CRITICAL(...)  ::terra::logger::get_client_logger()->critical(__VA_ARGS__)

#define TR_FILE_TRACE(...)     ::terra::logger::get_only_file_logger()->trace(__VA_ARGS__)
#define TR_FILE_DEBUG(...)     ::terra::logger::get_only_file_logger()->debug(__VA_ARGS__)
#define TR_FILE_INFO(...)      ::terra::logger::get_only_file_logger()->info(__VA_ARGS__)
#define TR_FILE_WARN(...)      ::terra::logger::get_only_file_logger()->warn(__VA_ARGS__)
#define TR_FILE_ERROR(...)     ::terra::logger::get_only_file_logger()->error(__VA_ARGS__)
#define TR_FILE_CRITICAL(...)  ::terra::logger::get_only_file_logger()->critical(__VA_ARGS__)
