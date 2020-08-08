#pragma once
#include <memory>
#include <optional>
#include <asio.hpp>
#include "../../common/source/Packet.h"

namespace ct
{
	template<typename Pipe = ct::Pipe<>>
	class Login
	{
	public:
		template<typename Duration>
		Login(std::shared_ptr<Pipe>, asio::io_context& io, Duration timeout);
		void OnError(std::function<void()>);
		void OnSuccess(std::function<void(uint64_t)>);

	private:
		void OnServerHello(Packet&&);
		void OnError();
		void OnSuccess(uint64_t id);
		void CleanUp();
		std::optional<uint64_t> ToNumber(const std::string&);

	public:
		std::shared_ptr<Pipe> pipe_;
		asio::steady_timer timeout_;

		std::function<void()> errorCallback_;
		std::function<void(uint64_t)> successCallback_;
	};

	template<typename Pipe>
	template<typename Duration>
	Login<Pipe>::Login(std::shared_ptr<Pipe> pipe, asio::io_context& io, Duration duration)
		:pipe_(pipe), timeout_(io, duration)
	{
		const Packet hello = std::string("hello from client");
		pipe_->SendPacket(hello);

		pipe_->OnPacket(
			[this](Packet&& packet)
		{
			OnServerHello(std::move(packet));
		});

		pipe_->OnBroken(
			[this]()
		{
			OnError();
		});

		timeout_.async_wait(
			[this](const std::error_code& error)
		{
			if (!error)
				OnError();
		});
	}

	template<typename Pipe>
	void Login<Pipe>::OnError(std::function<void()> callback)
	{
		errorCallback_ = callback;
	}

	template<typename Pipe>
	void Login<Pipe>::OnSuccess(std::function<void(uint64_t)> callback)
	{
		successCallback_ = callback;
	}

	template<typename Pipe>
	void Login<Pipe>::OnServerHello(Packet&& packet)
	{
		const std::string_view expected = "hello client, your id is ";
		auto reply = std::string(packet.Data(), packet.Size());
		if (reply.substr(0, expected.size()) != expected)
			return OnError();

		auto id = ToNumber(reply.substr(expected.size()));
		if (!id)
			return OnError();
		OnSuccess(*id);
	}

	template<typename Pipe>
	void Login<Pipe>::OnError()
	{
		auto callback = std::move(errorCallback_);
		CleanUp();
		callback();
	}

	template<typename Pipe>
	void Login<Pipe>::OnSuccess(uint64_t id)
	{
		auto callback = std::move(successCallback_);
		CleanUp();
		callback(id);
	}

	template<typename Pipe>
	void Login<Pipe>::CleanUp()
	{
		pipe_->OnPacket(nullptr);
		pipe_->OnBroken(nullptr);
		pipe_.reset();

		timeout_.cancel();

		successCallback_ = nullptr;
		errorCallback_ = nullptr;
	}

	template<typename Pipe>
	std::optional<uint64_t> Login<Pipe>::ToNumber(const std::string& str)
	{
		try
		{
			return std::stoull(str);
		}
		catch (...)
		{
			return std::nullopt;
		}
	}
}
