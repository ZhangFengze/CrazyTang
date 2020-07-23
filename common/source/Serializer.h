#pragma once
#include <istream>
#include <ostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace ct
{
	class OutputArchive
	{
	public:
		OutputArchive(std::ostream&);

		template<typename T>
		void Write(const T&);

		template<>
		void Write(const std::string_view&);

		template<>
		void Write(const std::string&);

	private:
		std::ostream& os_;
	};

	template<typename T>
	void OutputArchive::Write(const T& value)
	{
		os_.write(reinterpret_cast<const char*>(std::addressof(value)), sizeof(value));
	}

	template<>
	void OutputArchive::Write(const std::string_view& str)
	{
		size_t length = str.size();
		Write(length);
		os_.write(str.data(), length);
	}

	template<>
	void OutputArchive::Write(const std::string& value)
	{
		Write(std::string_view{ value });
	}

	class InputArchive
	{
	public:
		InputArchive(std::istream&);

		template<typename T>
		std::optional<T> Read();

		template<typename T>
		bool Read(T&);

		template<>
		bool Read(std::string&);

	private:
		std::istream& is_;
	};

	template<typename T>
	bool InputArchive::Read(T& value)
	{
		auto read = is_.readsome(reinterpret_cast<char*>(std::addressof(value)), sizeof(value));
		return read == sizeof(value);
	}

	template<>
	bool InputArchive::Read(std::string& value)
	{
		auto size = Read<size_t>();
		if (!size)
			return false;
		value.resize(*size);
		auto read = is_.readsome(value.data(), *size);
		return read == *size;
	}

	template<typename T>
	std::optional<T> InputArchive::Read()
	{
		T result;
		if (!Read(result))
			return std::nullopt;
		return result;
	}
}
