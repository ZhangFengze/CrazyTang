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

	private:
		std::shared_ptr<Pipe> pipe_;
		std::unordered_map<std::string, std::function<void(std::string&&)>> listeners_;
	};

	template<typename Pipe>
	PlayerAgent<Pipe>::PlayerAgent(std::shared_ptr<Pipe> pipe)
		:pipe_(pipe)
	{
		pipe_->OnPacket(
			[this](Packet&& packet)
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
		});
	}

	template<typename Pipe>
	void PlayerAgent<Pipe>::Listen(const std::string& tag, std::function<void(std::string&&)> handler)
	{
		assert(listeners_.find(tag) == listeners_.end());
		listeners_[tag] = handler;
	}
}
