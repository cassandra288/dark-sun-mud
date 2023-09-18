#include "dsspch.h"
#include "session.h"

#include <thread>
#include <iomanip>

namespace dss::gateway
{
	namespace
	{
		bool should_log_error(asio::error_code const& error)
		{
			if (error.category() == asio::ssl::error::stream_category && error.value() == asio::ssl::error::stream_truncated)
				return false;
			if (error.category() == asio::error::misc_category && error.value() == asio::error::eof)
				return false;
			if (error.category() == asio::error::system_category && error.value() == 1236) // The network connection was aborted by the local system.
				return false;
			return true;
		}
	}

	Session::Session(asio::io_context& io_service, asio::ssl::context& context, DisconnectCallback disconnect_callback, ReadyCallback ready_callback)
		: m_socket(io_service, context)
		, m_message_callbacks()
		, m_ready_callback(ready_callback)
		, m_disconnect_callback(disconnect_callback)
		, m_buffer()
		, m_last_message()
		, m_received_mutex()
		, m_received_cond()
	{
		m_id = generate_id("gses");
	}

	Session::~Session()
	{
		disconnect();
	}

	asio::ssl::stream<asio::ip::tcp::socket>::lowest_layer_type& Session::socket()
	{
		return m_socket.lowest_layer();
	}

	void Session::start()
	{
		DSS_GATEWAY_LOG_INFO("<{}> New inbound connection from {}, starting handshake", m_id, m_socket.lowest_layer().remote_endpoint().address().to_string());
		m_socket.async_handshake(asio::ssl::stream_base::server, std::bind(&Session::handle_handshake, this, std::placeholders::_1));
	}

	void Session::disconnect()
	{
		DSS_GATEWAY_LOG_INFO("<{}> Disconnecting", m_id);
		m_disconnect_callback(*this);
		m_socket.lowest_layer().close();
	}

	void Session::register_message_callback(MessageCallback callback)
	{
		m_message_callbacks.push_back(callback);
	}

	void Session::send_message(std::string message)
	{
		DSS_GATEWAY_LOG_INFO("<{}> Sending message: {}", m_id, message);
		asio::async_write(m_socket, asio::buffer(message),
			[this](asio::error_code const& error, size_t bytes_transferred)
			{
				if (error && should_log_error(error))
					DSS_GATEWAY_LOG_ERROR("<{}> Send failed: {}:{} ({})", m_id, error.category().name(), error.value(), error.message());
			});
	}

	std::string Session::wait_for_message()
	{
		std::unique_lock<std::mutex> lock(m_received_mutex);
		m_received_cond.wait(lock);
		return m_last_message;
	}
	
	void Session::handle_handshake(asio::error_code const& error)
	{
		if (!error)
		{
			DSS_GATEWAY_LOG_INFO("<{}> Successful handshake", m_id);
			m_ready_callback(*this);
			m_socket.async_read_some(asio::buffer(m_buffer, m_buffer.size()), std::bind(&Session::handle_response, this, std::placeholders::_1, std::placeholders::_2));
		}
		else
		{
			if (should_log_error(error))
				DSS_GATEWAY_LOG_ERROR("<{}> Handshake failed: {}:{} ({})", m_id, error.category().name(), error.value(), error.message());
			delete this;
		}
	}

	void Session::handle_response(asio::error_code const& error, size_t bytes_transferred)
	{
		if (!error)
		{
			std::string str(m_buffer.data(), bytes_transferred);

			int i = str.find("\r\n");
			while (i != str.npos) {
				str = str.replace(i, 2, "\n");
				i = str.find("\r\n");
			}
			if (str[str.size() - 1] == '\n') {
				str = str.substr(0, str.size() - 1);
			}

			m_last_message = str;
			m_received_cond.notify_all();
			for (auto& callback : m_message_callbacks)
			{
				callback(*this, str);
			}

			m_socket.async_read_some(asio::buffer(m_buffer, m_buffer.size()), std::bind(&Session::handle_response, this, std::placeholders::_1, std::placeholders::_2));
		}
		else
		{
			if (should_log_error(error))
				DSS_GATEWAY_LOG_ERROR("<{}> Reading response failed: {}:{} ({})", m_id, error.category().name(), error.value(), error.message());
			delete this;
		}
	}
}
