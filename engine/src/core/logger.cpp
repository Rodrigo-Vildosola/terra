#include "terra/core/logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

namespace terra {

    std::shared_ptr<spdlog::logger> logger::core_logger;
    std::shared_ptr<spdlog::logger> logger::client_logger;
    std::shared_ptr<spdlog::logger> logger::only_file_logger;


    void logger::init()
    {
        std::vector<spdlog::sink_ptr> log_sinks;
        log_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        log_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("terra.log", true));

        log_sinks[0]->set_pattern("%^[%T] %n: %v%$");
        log_sinks[1]->set_pattern("[%T] [%l] %n: %v");

        core_logger = std::make_shared<spdlog::logger>("TERRA", begin(log_sinks), end(log_sinks));
        spdlog::register_logger(core_logger);
        core_logger->set_level(spdlog::level::trace);
        core_logger->flush_on(spdlog::level::trace);

        client_logger = std::make_shared<spdlog::logger>("APP", begin(log_sinks), end(log_sinks));
        spdlog::register_logger(client_logger);
        client_logger->set_level(spdlog::level::trace);
        client_logger->flush_on(spdlog::level::trace);

        only_file_logger = std::make_shared<spdlog::logger>("TERRA_F", log_sinks[1]);
        spdlog::register_logger(only_file_logger);
        only_file_logger->set_level(spdlog::level::trace);
    }
}
