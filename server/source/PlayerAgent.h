#pragma once
#include "../../common/source/Packet.h"
#include "../../common/source/Pipe.h"
#include "../../common/source/Serializer.h"
#include <string>
#include <functional>
#include <cassert>

namespace ct
{
	template<typename Pipe = ct::Pipe<>>
	class PlayerAgent
	{
	public:
		PlayerAgent(std::shared_ptr<Pipe>);
		void Listen(const std::string& tag, std::function<void(std::string&&)>);
		void OnError(std::function<void()>);

	private:
		void OnPacket(Packet&&);

	private:
		std::shared_ptr<Pipe> pipe_;
		std::function<void()> errorHandler_;
		std::unordered_map<std::string, std::function<void(std::string&&)>> listeners_;
	};

	template<typename Pipe>
	PlayerAgent<Pipe>::PlayerAgent(std::shared_ptr<Pipe> pipe)
		:pipe_(pipe)
	{
		pipe_->OnPacket(
			[this](Packet&& packet)
		{
			OnPacket(std::move(packet));
		});

		pipe_->OnBroken([this]()
		{
			errorHandler_();
		});
	}

	template<typename Pipe>
	void PlayerAgent<Pipe>::Listen(const std::string& tag, std::function<void(std::string&&)> handler)
	{
		assert(listeners_.find(tag) == listeners_.end());
		listeners_[tag] = handler;
	}

	template<typename Pipe>
	void PlayerAgent<Pipe>::OnError(std::function<void()> handler)
	{
		errorHandler_ = handler;
	}

	template<typename Pipe>
	void PlayerAgent<Pipe>::OnPacket(Packet&& packet)
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
}
