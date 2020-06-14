//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
#include "toml_default_formatter.h"
//# {{
#if !TOML_DOXYGEN
#if !defined(TOML_IMPLEMENTATION) || !TOML_IMPLEMENTATION
	#error This is an implementation-only header.
#endif
//# }}

TOML_PUSH_WARNINGS
TOML_DISABLE_SWITCH_WARNINGS
TOML_DISABLE_FLOAT_WARNINGS

namespace toml::impl
{
	TOML_PUSH_WARNINGS
	TOML_DISABLE_ALL_WARNINGS

	TOML_API
	TOML_EXTERNAL_LINKAGE
	toml::string default_formatter_make_key_segment(const toml::string& str) noexcept
	{
		if (str.empty())
			return TOML_STRING_PREFIX("''"s);
		else
		{
			bool requiresQuotes = false;
			{
				impl::utf8_decoder decoder;
				for (size_t i = 0; i < str.length() && !requiresQuotes; i++)
				{
					decoder(static_cast<uint8_t>(str[i]));
					if (decoder.error())
						requiresQuotes = true;
					else if (decoder.has_code_point())
						requiresQuotes = !impl::is_bare_key_character(decoder.codepoint);
				}
			}

			if (requiresQuotes)
			{
				toml::string s;
				s.reserve(str.length() + 2_sz);
				s += TOML_STRING_PREFIX('"');
				for (auto c : str)
				{
					if TOML_UNLIKELY(c >= TOML_STRING_PREFIX('\x00') && c <= TOML_STRING_PREFIX('\x1F'))
						s.append(low_character_escape_table[c]);
					else if TOML_UNLIKELY(c == TOML_STRING_PREFIX('\x7F'))
						s.append(TOML_STRING_PREFIX("\\u007F"sv));
					else if TOML_UNLIKELY(c == TOML_STRING_PREFIX('"'))
						s.append(TOML_STRING_PREFIX("\\\""sv));
					else
						s += c;
				}
				s += TOML_STRING_PREFIX('"');
				return s;
			}
			else
				return str;
		}
	}

	TOML_POP_WARNINGS

	TOML_API
	TOML_EXTERNAL_LINKAGE
	size_t default_formatter_inline_columns(const node& node) noexcept
	{
		switch (node.type())
		{
			case node_type::table:
			{
				auto& n = *reinterpret_cast<const table*>(&node);
				if (n.empty())
					return 2_sz; // "{}"
				size_t weight = 3_sz; // "{ }"
				for (auto [k, v] : n)
					weight += k.length() + default_formatter_inline_columns(v) + 2_sz; // +  ", "
				return weight;
			}

			case node_type::array:
			{
				auto& n = *reinterpret_cast<const array*>(&node);
				if (n.empty())
					return 2_sz; // "[]"
				size_t weight = 3_sz; // "[ ]"
				for (auto& elem : n)
					weight += default_formatter_inline_columns(elem) + 2_sz; // +  ", "
				return weight;
			}

			case node_type::string:
			{
				auto& n = *reinterpret_cast<const value<string>*>(&node);
				return n.get().length() + 2_sz; // + ""
			}

			case node_type::integer:
			{
				auto& n = *reinterpret_cast<const value<int64_t>*>(&node);
				auto v = n.get();
				if (!v)
					return 1_sz;
				size_t weight = {};
				if (v < 0)
				{
					weight += 1;
					v *= -1;
				}
				return weight + static_cast<size_t>(log10(static_cast<double>(v))) + 1_sz;
			}

			case node_type::floating_point:
			{
				auto& n = *reinterpret_cast<const value<double>*>(&node);
				auto v = n.get();
				if (v == 0.0)
					return 3_sz;  // "0.0"
				size_t weight = 2_sz; // ".0"
				if (v < 0.0)
				{
					weight += 1;
					v *= -1.0;
				}
				return weight + static_cast<size_t>(log10(static_cast<double>(v))) + 1_sz;
				break;
			}

			case node_type::boolean: return 5_sz;
			case node_type::date: [[fallthrough]];
			case node_type::time: return 10_sz;
			case node_type::date_time: return 30_sz;
			case node_type::none: TOML_UNREACHABLE;
			TOML_NO_DEFAULT_CASE;
		}

		TOML_UNREACHABLE;
	}

	TOML_API
	TOML_EXTERNAL_LINKAGE
	bool default_formatter_forces_multiline(const node& node, size_t starting_column_bias) noexcept
	{
		return (default_formatter_inline_columns(node) + starting_column_bias) > 120_sz;
	}
}

namespace toml
{
	template <typename Char>
	TOML_EXTERNAL_LINKAGE
	void default_formatter<Char>::print_inline(const toml::table& tbl)
	{
		if (tbl.empty())
			impl::print_to_stream("{}"sv, base::stream());
		else
		{
			impl::print_to_stream("{ "sv, base::stream());

			bool first = false;
			for (auto [k, v] : tbl)
			{
				if (first)
					impl::print_to_stream(", "sv, base::stream());
				first = true;

				print_key_segment(k);
				impl::print_to_stream(" = "sv, base::stream());

				const auto type = v.type();
				TOML_ASSUME(type != node_type::none);
				switch (type)
				{
					case node_type::table: print_inline(*reinterpret_cast<const table*>(&v)); break;
					case node_type::array: print(*reinterpret_cast<const array*>(&v)); break;
					default:
						base::print_value(v, type);
				}
			}

			impl::print_to_stream(" }"sv, base::stream());
		}
		base::clear_naked_newline();
	}
}

TOML_POP_WARNINGS // TOML_DISABLE_SWITCH_WARNINGS, TOML_DISABLE_FLOAT_WARNINGS

//# {{
#endif // !TOML_DOXYGEN
//# }}
