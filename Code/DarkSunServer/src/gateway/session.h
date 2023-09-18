#pragma once

namespace dss::gateway
{
	class Session
	{
		friend class Server;

	public:
		typedef std::function<void(Session&, std::string)> MessageCallback;
		typedef std::function<void(Session&)> ReadyCallback;
		
		inline bool operator ==(const Session& rhs)
		{
			return m_id == rhs.m_id;
		};

		asio::ssl::stream<asio::ip::tcp::socket>::lowest_layer_type& socket();

		inline std::string const& id() const { return m_id; }
		inline std::string const& last_message() const { return m_last_message; }

		void start();
		void disconnect();
		void register_message_callback(MessageCallback callback);
		void send_message(std::string message);

		std::string wait_for_message();

	private:
		typedef std::function<void(Session&)> DisconnectCallback;
		
		Session(asio::io_context& io_service, asio::ssl::context& context, DisconnectCallback disconnect_callback, ReadyCallback ready_callback);
		~Session();
		
		std::string m_id;
		asio::ssl::stream<asio::ip::tcp::socket> m_socket;
		std::array<char, 8192> m_buffer;
		std::vector<MessageCallback> m_message_callbacks;
		ReadyCallback m_ready_callback;
		DisconnectCallback m_disconnect_callback;
		std::string m_last_message;
		std::mutex m_received_mutex;
		std::condition_variable m_received_cond;

		void handle_handshake(asio::error_code const& error);
		void handle_response(asio::error_code const& error, size_t bytes_transferred);
	};
}
