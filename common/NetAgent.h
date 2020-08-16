#pragma once
#include "Packet.h"
#include "Pipe.h"
#include "Archive.h"
#include <string>
#include <functional>
#include <cassert>

namespace ct
{
	template<typename Pipe = ct::Pipe<>>
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
		OutputStringArchive ar;
		ar.Write(tag);
		ar.Write(content);
		pipe_->SendPacket(Packet{ ar.String() });
	}

	template<typename Pipe>
	void NetAgent<Pipe>::OnPacket(Packet&& packet)
	{
		InputStringArchive ar(std::string{ packet.Data(),packet.Size() });
		auto tag = ar.Read<std::string>();
		if (!tag)
			return;

		auto iter = listeners_.find(*tag);
		if (iter == listeners_.end())
			return;

		auto content = ar.Read<std::string>();
		if (!content)
			return;

		iter->second(std::move(*content));
	}

	template<typename Pipe>
	void NetAgent<Pipe>::CleanUp()
	{
		pipe_.reset();
		errorHandler_ = nullptr;
		listeners_.clear();
	}
}
