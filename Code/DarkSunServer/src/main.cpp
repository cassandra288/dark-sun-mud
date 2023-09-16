#include "dsspch.h"
#include "gateway/server.h"

dss::gateway::Server* s;

void on_message(dss::gateway::Session& session, std::string message)
{
	DSS_LOG_TRACE("Message from {}: {}", session.id(), message);
	s->broadcast_message(message);
	
}

int main()
{
	dss::Log::init();

	asio::io_service io_service;

	s = new dss::gateway::Server(io_service, 5252, [](dss::gateway::Session& session) {
		DSS_LOG_TRACE("New Session: {}", session.id());
		session.register_message_callback(on_message);
	});

	io_service.run();

	delete s;

	return 0;
}
