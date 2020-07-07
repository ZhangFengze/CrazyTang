#pragma once
#include <asio.hpp>
#include "../../common/source/Connection.h"

namespace ct
{
	class Net
	{
	public:
		enum class State
		{
			Invalid,
			Connecting,
			Working,
		};

		Net(asio::io_context& io);
		bool Working() const;
		void Connect(const asio::ip::tcp::endpoint& server);
		void Send(const char*, size_t);
		void OnData(const std::function<void(const char*, size_t)>&);

	private:
		void Read();

	private:
		State state_ = State::Invalid;
		asio::io_context& io_;
		std::shared_ptr<Connection> connection_;
		std::function<void(const char*, size_t)> onData_;
		std::shared_ptr<bool> alive_ = std::make_shared<bool>(true);
	};
}
