#pragma once
#include "Packet.h"
#include "Net.h"
#include <ZSerializer.hpp>
#include <string>
#include <functional>
#include <cassert>

namespace ct
{
	class NetAgent2
	{
	public:
		NetAgent2(asio::ip::tcp::socket socket)
			:socket_(std::move(socket)) {}

		void Listen(const std::string& tag, std::function<void(std::string&&)> handler)
		{
			assert(listeners_.find(tag) == listeners_.end());
			listeners_[tag] = handler;
		}

		void OnError(std::function<void()> handler)
		{
			errorHandler_ = handler;
		}

		void Send(const std::string& tag, const std::string& content)
		{
			zs::StringWriter out;
			zs::Write(out, tag);
			zs::Write(out, content);
			out_.emplace_back(Packet{ out.String() });
		}

		asio::awaitable<void> ReadRoutine()
		{
			while (socket_.is_open())
			{
				auto packet = co_await AsyncReadPacket(socket_);
				zs::StringReader ar(std::string{ packet.Data(),packet.Size() });
				auto tag = zs::Read<std::string>(ar);
				if (std::holds_alternative<zs::Error>(tag))
					co_return;

				auto iter = listeners_.find(std::get<0>(tag));
				if (iter == listeners_.end())
					continue;

				auto content = zs::Read<std::string>(ar);
				if (std::holds_alternative<zs::Error>(content))
					co_return;

				iter->second(std::move(std::get<0>(content)));
			}
		}

		asio::awaitable<void> WriteRoutine()
		{
			while (socket_.is_open())
			{
				// TODO optimize
				if (out_.empty())
				{
					asio::steady_timer t{ co_await asio::this_coro::executor };
					t.expires_after(std::chrono::milliseconds{ 0 });
					co_await t.async_wait(asio::use_awaitable);
					continue;
				}

				auto packet = out_.front();
				out_.pop_front();
				co_await AsyncWritePacket(socket_, std::move(packet));
			}
		}

	private:
		asio::ip::tcp::socket socket_;
		std::unordered_map<std::string, std::function<void(std::string&&)>> listeners_;
		std::function<void()> errorHandler_;// TODO
		std::list<Packet> out_;
	};

	template<typename Pipe>
	class NetAgent
	{
	public:
		NetAgent(std::shared_ptr<Pipe>);

		void Listen(const std::string& tag, std::function<void(std::string&&)>);
		void OnError(std::function<void()>);
		
		void Send(const std::string& tag, const std::string& content);

	private:
		void OnPacket(Packet&&);
		void CleanUp();

	private:
		std::shared_ptr<Pipe> pipe_;
		std::function<void()> errorHandler_;
		std::unordered_map<std::string, std::function<void(std::string&&)>> listeners_;
	};

	template<typename Pipe>
	NetAgent<Pipe>::NetAgent(std::shared_ptr<Pipe> pipe)
		:pipe_(pipe)
	{
		pipe_->OnPacket(
			[this](Packet&& packet)
		{
			OnPacket(std::move(packet));
		});

		pipe_->OnBroken([this]()
		{
			auto callback = std::move(errorHandler_);
			CleanUp();
			callback();
		});
	}

	template<typename Pipe>
	void NetAgent<Pipe>::Listen(const std::string& tag, std::function<void(std::string&&)> handler)
	{
		assert(listeners_.find(tag) == listeners_.end());
		listeners_[tag] = handler;
	}

	template<typename Pipe>
	void NetAgent<Pipe>::OnError(std::function<void()> handler)
	{
		errorHandler_ = handler;
	}

	template<typename Pipe>
	void NetAgent<Pipe>::Send(const std::string& tag, const std::string& content)
	{
		zs::StringWriter out;
		zs::Write(out, tag);
		zs::Write(out, content);
		pipe_->SendPacket(Packet{ out.String() });
	}

	template<typename Pipe>
	void NetAgent<Pipe>::OnPacket(Packet&& packet)
	{
		zs::StringReader ar(std::string{ packet.Data(),packet.Size() });
		auto tag = zs::Read<std::string>(ar);
		if (std::holds_alternative<zs::Error>(tag))
			return;

		auto iter = listeners_.find(std::get<0>(tag));
		if (iter == listeners_.end())
			return;

		auto content = zs::Read<std::string>(ar);
		if (std::holds_alternative<zs::Error>(content))
			return;

		iter->second(std::move(std::get<0>(content)));
	}

	template<typename Pipe>
	void NetAgent<Pipe>::CleanUp()
	{
		pipe_.reset();
		errorHandler_ = nullptr;
		listeners_.clear();
	}
}
