#pragma once
#include <memory>
#include <asio.hpp>
#include "../../common/source/Packet.h"

namespace ct
{
	template<typename Pipe>
	class Login
	{
	public:
		template<typename Duration>
		Login(std::shared_ptr<Pipe>, uint64_t id, asio::io_context& io, Duration timeout);
		void OnError(std::function<void()>);
		void OnSuccess(std::function<void()>);

	private:
		void OnClientHello(Packet&&);
		void OnError();
		void OnSuccess();
		void CleanUp();

	public:
		std::shared_ptr<Pipe> pipe_;
		uint64_t id_;
		asio::steady_timer timeout_;

		std::function<void()> errorCallback_;
		std::function<void()> successCallback_;
	};

	template<typename Pipe>
	template<typename Duration>
	Login<Pipe>::Login(std::shared_ptr<Pipe> pipe, uint64_t id, asio::io_context& io, Duration duration)
		:pipe_(pipe), id_(id), timeout_(io, duration)
	{
		pipe_->OnPacket(
			[this](Packet&& packet)
		{
			OnClientHello(std::move(packet));
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
	void Login<Pipe>::OnSuccess(std::function<void()> callback)
	{
		successCallback_ = callback;
	}

	template<typename Pipe>
	void Login<Pipe>::OnClientHello(Packet&& packet)
	{
		const Packet expected = std::string("hello from client");
		if (packet != expected)
			return OnError();
		auto reply = "hello client, your id is " + std::to_string(id_);
		pipe_->SendPacket(Packet{ reply });
		OnSuccess();
	}

	template<typename Pipe>
	void Login<Pipe>::OnError()
	{
		auto callback = std::move(errorCallback_);
		CleanUp();
		callback();
	}

	template<typename Pipe>
	void Login<Pipe>::OnSuccess()
	{
		auto callback = std::move(successCallback_);
		CleanUp();
		callback();
	}

	template<typename Pipe>
	void Login<Pipe>::CleanUp()
	{
		pipe_->OnPacket(nullptr);
		pipe_->OnBroken(nullptr);
		pipe_.reset();

		id_ = -1;

		timeout_.cancel();

		successCallback_ = nullptr;
		errorCallback_ = nullptr;
	}
}
