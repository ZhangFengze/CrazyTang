#include "Net.h"
#include "Game.h"

namespace ct
{
	Net::Net(asio::io_context& io)
		:io_(io)
	{
	}

	bool Net::Working() const
	{
		return state_ == State::Working;
	}

	void Net::Connect(const asio::ip::tcp::endpoint& server)
	{
		assert(state_ == State::Invalid);
		assert(!connection_);

		connection_ = std::make_shared<Connection>(asio::ip::tcp::socket{ io_ });
		std::weak_ptr<Connection> alive = connection_;

		connection_->socket_.async_connect(server,
			[alive, this](const std::error_code& error)
		{
			if (alive.expired())
				return;
			assert(state_ == State::Connecting);
			if (error)
			{
				state_ = State::Invalid;
				return;
			}
			state_ = State::Working;
			Read();
		});

		state_ = State::Connecting;
	}

	void Net::Send(const char* data, size_t size)
	{
		assert(state_ == State::Working);
		assert(connection_);

		std::weak_ptr<Connection> alive = connection_;
		connection_->AsyncWritePacket(data, size,
			[alive, this](const std::error_code& error)
		{
			if (alive.expired())
				return;
			assert(state_ == State::Working);
			if (error)
			{
				connection_.reset();
				state_ = State::Invalid;
				return;
			}
		});
	}

	void Net::OnData(const std::function<void(const char*, size_t)>& function)
	{
		onData_ = function;
	}

	void Net::Read()
	{
		assert(state_ == State::Working);
		assert(connection_);

		std::weak_ptr<Connection> alive = connection_;
		connection_->AsyncReadPacket(
			[alive, this](const std::error_code& error, const char* data, size_t size)
		{
			if (alive.expired())
				return;
			assert(state_ == State::Working);
			if (error)
			{
				connection_.reset();
				state_ = State::Invalid;
				return;
			}
			if (onData_)
				onData_(data, size);
			Read();
		});
	}
}