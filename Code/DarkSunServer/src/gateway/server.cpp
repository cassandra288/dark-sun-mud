#include "dsspch.h"
#include "server.h"

namespace dss::gateway
{
	Server::Server(asio::io_context& io_service, uint16_t port, NewSessionCallback callback)
		: m_io_context(io_service)
		, m_acceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v6(), port))
		, m_context(asio::ssl::context::sslv23)
		, m_sessions()
		, m_new_session_callback(callback)
	{
		m_context.set_options(
			asio::ssl::context::default_workarounds
			| asio::ssl::context::no_sslv2
			| asio::ssl::context::no_sslv3
			| asio::ssl::context::single_dh_use);

		m_context.set_password_callback(std::bind(&Server::get_password, this));
		m_context.use_certificate_chain_file("cert.pem");
		m_context.use_private_key_file("key.pem", asio::ssl::context::pem);

		auto new_session = new Session(m_io_context, m_context, std::bind(&Server::handle_disconnect, this, std::placeholders::_1));
		m_acceptor.async_accept(new_session->socket(), std::bind(&Server::handle_accept, this, new_session, std::placeholders::_1));
	}

	std::string Server::get_password() const
	{
		// TODO: Implement a secure way to store and fetch the cert password.
		// This is currently only used for testing purposes and should not stay this way.
		return "1111";
	}

	void Server::handle_accept(Session* new_session, asio::error_code const& error)
	{
		if (!error)
		{;
			new_session->start();
			m_new_session_callback(*new_session);
			m_sessions.push_back(new_session);
			
			new_session = new Session(m_io_context, m_context, std::bind(&Server::handle_disconnect, this, std::placeholders::_1));
			m_acceptor.async_accept(new_session->socket(), std::bind(&Server::handle_accept, this, new_session, std::placeholders::_1));
		}
		else
		{
			DSS_GATEWAY_LOG_ERROR("Error accepting connection: {}", error.message());
			delete new_session;
			new_session = new Session(m_io_context, m_context, std::bind(&Server::handle_disconnect, this, std::placeholders::_1));
			m_acceptor.async_accept(new_session->socket(), std::bind(&Server::handle_accept, this, new_session, std::placeholders::_1));
		}
	}

	void Server::handle_disconnect(Session& session)
	{
		auto it = std::find_if(m_sessions.begin(), m_sessions.end(), [&session](Session* s) { return (*s) == session; });
		if (it != m_sessions.end())
		{
			m_sessions.erase(it);
		}
	}

	void Server::broadcast_message(std::string message)
	{
		for (auto session : m_sessions)
		{
			session->send_message(message);
		}
	}
}
