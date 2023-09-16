#pragma once

namespace dss::gateway
{
	class Session
	{
		friend class Server;

	public:
		typedef std::function<void(Session&, std::string)> MessageCallback;
		
		inline bool operator ==(const Session& rhs)
		{
			return m_id == rhs.m_id;
		};

		asio::ssl::stream<asio::ip::tcp::socket>::lowest_layer_type& socket();
		
		inline std::string const& id() const { return m_id; }

		void start();
		void disconnect();
		void register_message_callback(MessageCallback callback);
		void send_message(std::string message);

	private:
		typedef std::function<void(Session&)> disconnect_callback;
		
		Session(asio::io_context& io_service, asio::ssl::context& context, disconnect_callback disconnect_callback);
		~Session();
		
		std::string m_id;
		asio::ssl::stream<asio::ip::tcp::socket> m_socket;
		std::array<char, 8192> m_buffer;
		std::vector<MessageCallback> m_message_callbacks;
		disconnect_callback m_disconnect_callback;

		void handle_handshake(asio::error_code const& error);
		void handle_response(asio::error_code const& error, size_t bytes_transferred);
	};
}
