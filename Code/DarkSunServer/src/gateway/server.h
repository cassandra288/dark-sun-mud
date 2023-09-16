#pragma once

#include "session.h"

namespace dss::gateway
{
	class Server
	{
	public:
		typedef std::function<void(Session&)> NewSessionCallback;
		
		Server(asio::io_service& io_service, uint16_t port, NewSessionCallback callback);

		void broadcast_message(std::string message);
	private:
		asio::io_context& m_io_context;
		asio::ip::tcp::acceptor m_acceptor;
		asio::ssl::context m_context;
		std::vector<Session*> m_sessions;
		NewSessionCallback m_new_session_callback;
		
		std::string get_password() const;

		void handle_accept(Session* new_session, asio::error_code const& error);
		void handle_disconnect(Session& session);
	};
}
