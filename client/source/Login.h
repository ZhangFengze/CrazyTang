#pragma once
#include <memory>
#include <asio.hpp>

namespace ct
{
	template<typename Pipe>
	class Login
	{
	public:
		template<typename Duration>
		Login(std::shared_ptr<Pipe>, asio::io_context& io, Duration timeout);
		void OnError(std::function<void()>);
		void OnSuccess(std::function<void(uint64_t)>);

	private:
		void OnServerHello(const char*, size_t);
		void OnError();
		void OnSuccess(uint64_t id);
		void CleanUp();

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
		const std::string_view hello = "hello from client";
		pipe_->SendPacket(hello.data(), hello.size());

		pipe_->OnPacket(
			[this](const char* data, size_t size)
		{
			OnServerHello(data, size);
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
	void Login<Pipe>::OnServerHello(const char* data, size_t size)
	{
		const std::string_view expected = "hello client, your id is ";
		auto reply = std::string(data, size);
		if (reply.substr(0, expected.size()) != expected)
			return OnError();

		try
		{
			uint64_t id = std::stoull(reply.substr(expected.size()));
			OnSuccess(id);
		}
		catch (...)
		{
			OnError();
		}
	}

	template<typename Pipe>
	void Login<Pipe>::OnError()
	{
		CleanUp();
		errorCallback_();
	}

	template<typename Pipe>
	void Login<Pipe>::OnSuccess(uint64_t id)
	{
		CleanUp();
		successCallback_(id);
	}

	template<typename Pipe>
	void Login<Pipe>::CleanUp()
	{
		pipe_->OnPacket(nullptr);
		pipe_->OnBroken(nullptr);
		timeout_.cancel();
	}
}
