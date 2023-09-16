#pragma once


namespace dss
{
	class Log
	{
	public:
		static void init();

		inline static std::shared_ptr<spdlog::logger>& get_core_logger() { return s_core_logger; }
		inline static std::shared_ptr<spdlog::logger>& get_gateway_logger() { return s_gateway_logger; }
	private:
		static std::shared_ptr<spdlog::logger> s_core_logger;
		static std::shared_ptr<spdlog::logger> s_gateway_logger;
	};
}

// Core log macros
#define DSS_LOG_TRACE(...)    ::dss::Log::get_core_logger()->trace(__VA_ARGS__)
#define DSS_LOG_INFO(...)     ::dss::Log::get_core_logger()->info(__VA_ARGS__)
#define DSS_LOG_WARN(...)     ::dss::Log::get_core_logger()->warn(__VA_ARGS__)
#define DSS_LOG_ERROR(...)    ::dss::Log::get_core_logger()->error(__VA_ARGS__)
#define DSS_LOG_CRITICAL(...) ::dss::Log::get_core_logger()->critical(__VA_ARGS__)

// Gateway log macros
#define DSS_GATEWAY_LOG_TRACE(...)    ::dss::Log::get_gateway_logger()->trace(__VA_ARGS__)
#define DSS_GATEWAY_LOG_INFO(...)     ::dss::Log::get_gateway_logger()->info(__VA_ARGS__)
#define DSS_GATEWAY_LOG_WARN(...)     ::dss::Log::get_gateway_logger()->warn(__VA_ARGS__)
#define DSS_GATEWAY_LOG_ERROR(...)    ::dss::Log::get_gateway_logger()->error(__VA_ARGS__)
#define DSS_GATEWAY_LOG_CRITICAL(...) ::dss::Log::get_gateway_logger()->critical(__VA_ARGS__)
