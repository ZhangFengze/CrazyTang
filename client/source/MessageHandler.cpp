#include "MessageHandler.h"
#include <cassert>

namespace ct
{
	void MessageHandler::Register(const std::string_view& tag, const std::function<void(const std::string&)> handler)
	{
		assert(handlers_.find(tag) == handlers_.end());
		handlers_[tag] = handler;
	}

	void MessageHandler::UnRegister(const std::string_view& tag)
	{
		assert(handlers_.find(tag) != handlers_.end());
		handlers_.erase(tag);
	}

	void MessageHandler::OnMessage(const std::string& message)
	{
		auto tagPos = message.find_first_of(' ');
		auto tag = tagPos == std::string::npos ? message : message.substr(0, tagPos);

		auto iter = handlers_.find(tag);
		if (iter == handlers_.end())
			return;
		iter->second(message);
	}
}