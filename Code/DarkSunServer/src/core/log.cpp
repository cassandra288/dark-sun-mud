#include "dsspch.h"
#include "log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#ifdef DSS_DEBUG
#define LOG_LEVEL spdlog::level::trace
#else
#define LOG_LEVEL spdlog::level::info
#endif

namespace dss
{
	std::shared_ptr<spdlog::logger> Log::s_core_logger;
	std::shared_ptr<spdlog::logger> Log::s_gateway_logger;

	void Log::init()
	{
		std::vector<spdlog::sink_ptr> log_sinks;
		log_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		log_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.txt", true));

		s_core_logger = std::make_shared<spdlog::logger>("CORE", log_sinks.begin(), log_sinks.end());
		s_core_logger->set_level(LOG_LEVEL);
		s_core_logger->flush_on(spdlog::level::trace);

		s_gateway_logger = std::make_shared<spdlog::logger>("GATEWAY", log_sinks.begin(), log_sinks.end());
		s_gateway_logger->set_level(LOG_LEVEL);
		s_gateway_logger->flush_on(spdlog::level::trace);
	}
}
