#pragma once
#include <string>
#include <functional>

namespace ct
{
	class MessageHandler
	{
	public:
		void Register(const std::string_view& tag, const std::function<void(const std::string&)> handler);
		void UnRegister(const std::string_view& tag);
		void OnMessage(const std::string&);
	private:
		std::unordered_map<std::string_view, std::function<void(const std::string&)>> handlers_;
	};
}
