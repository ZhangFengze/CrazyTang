//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
//# {{
#include "toml_preprocessor.h"
#if !TOML_PARSER
	#error This header cannot not be included when TOML_PARSER is disabled.
#endif
//# }}
#include "toml_table.h"
#include "toml_utf8_streams.h"

TOML_PUSH_WARNINGS
TOML_DISABLE_PADDING_WARNINGS

namespace toml
{
	#if TOML_DOXYGEN || !TOML_EXCEPTIONS

	TOML_ABI_NAMESPACE_START(parse_noex)

	/// \brief	The result of a parsing operation.
	///
	/// \detail A parse_result is effectively a discriminated union containing either a toml::table
	/// 		or a toml::parse_error. Most member functions assume a particular one of these two states,
	/// 		and calling them when in the wrong state will cause errors (e.g. attempting to access the
	/// 		error object when parsing was successful). \cpp
	/// parse_result result = toml::parse_file("config.toml");
	/// if (result)
	///		do_stuff_with_a_table(result); //implicitly converts to table&
	///	else
	///		std::cerr << "Parse failed:\n"sv << result.error() << std::endl;
	/// 
	/// \ecpp
	/// 	
	/// \out
	/// example output:
	/// 
	/// Parse failed:
	/// Encountered unexpected character while parsing boolean; expected 'true', saw 'trU'
	///		(error occurred at line 1, column 13 of 'config.toml')
	/// \eout
	/// 
	/// Getting node_views (`operator[]`) and using the iterator accessor functions (`begin(), end()` etc.) are
	/// unconditionally safe; when parsing fails these just return 'empty' values. A ranged-for loop on a failed
	/// parse_result is also safe since `begin()` and `end()` return the same iterator and will not lead to any
	/// dereferences and iterations.
	/// 
	/// \attention <strong>This type only exists when exceptions are not enabled.</strong>
	/// 		 Otherwise parse_result is just an alias for toml::table: \cpp
	/// #if TOML_EXCEPTIONS
	///		using parse_result = table;
	/// #else
	///		class parse_result final { // ...
	///	#endif
	/// \ecpp
	class parse_result final
	{
		private:
			std::aligned_storage_t<
				(sizeof(table) < sizeof(parse_error) ? sizeof(parse_error) : sizeof(table)),
				(alignof(table) < alignof(parse_error) ? alignof(parse_error) : alignof(table))
			> storage;
			bool is_err;

			void destroy() noexcept
			{
				if (is_err)
					TOML_LAUNDER(reinterpret_cast<parse_error*>(&storage))->~parse_error();
				else
					TOML_LAUNDER(reinterpret_cast<table*>(&storage))->~table();
			}

		public:

			/// \brief A BidirectionalIterator for iterating over key-value pairs in a wrapped toml::table.
			using iterator = table_iterator;

			/// \brief A BidirectionalIterator for iterating over const key-value pairs in a wrapped toml::table.
			using const_iterator = const_table_iterator;

			/// \brief	Returns true if parsing succeeeded.
			[[nodiscard]] bool succeeded() const noexcept { return !is_err; }
			/// \brief	Returns true if parsing failed.
			[[nodiscard]] bool failed() const noexcept { return is_err; }
			/// \brief	Returns true if parsing succeeeded.
			[[nodiscard]] explicit operator bool() const noexcept { return !is_err; }

			/// \brief	Returns the internal toml::table.
			[[nodiscard]] table& get() & noexcept
			{
				TOML_ASSERT(!is_err);
				return *TOML_LAUNDER(reinterpret_cast<table*>(&storage));
			}
			/// \brief	Returns the internal toml::table (rvalue overload).
			[[nodiscard]] table&& get() && noexcept
			{
				TOML_ASSERT(!is_err);
				return std::move(*TOML_LAUNDER(reinterpret_cast<table*>(&storage)));
			}
			/// \brief	Returns the internal toml::table (const lvalue overload).
			[[nodiscard]] const table& get() const& noexcept
			{
				TOML_ASSERT(!is_err);
				return *TOML_LAUNDER(reinterpret_cast<const table*>(&storage));
			}

			/// \brief	Returns the internal toml::parse_error.
			[[nodiscard]] parse_error& error() & noexcept
			{
				TOML_ASSERT(is_err);
				return *TOML_LAUNDER(reinterpret_cast<parse_error*>(&storage));
			}
			/// \brief	Returns the internal toml::parse_error (rvalue overload).
			[[nodiscard]] parse_error&& error() && noexcept
			{
				TOML_ASSERT(is_err);
				return std::move(*TOML_LAUNDER(reinterpret_cast<parse_error*>(&storage)));
			}
			/// \brief	Returns the internal toml::parse_error (const lvalue overload).
			[[nodiscard]] const parse_error& error() const& noexcept
			{
				TOML_ASSERT(is_err);
				return *TOML_LAUNDER(reinterpret_cast<const parse_error*>(&storage));
			}

			/// \brief	Returns the internal toml::table.
			[[nodiscard]] operator table& () noexcept { return get(); }
			/// \brief	Returns the internal toml::table (rvalue overload).
			[[nodiscard]] operator table&& () noexcept { return std::move(get()); }
			/// \brief	Returns the internal toml::table (const lvalue overload).
			[[nodiscard]] operator const table& () const noexcept { return get(); }

			/// \brief	Returns the internal toml::parse_error.
			[[nodiscard]] explicit operator parse_error& () noexcept { return error(); }
			/// \brief	Returns the internal toml::parse_error (rvalue overload).
			[[nodiscard]] explicit operator parse_error && () noexcept { return std::move(error()); }
			/// \brief	Returns the internal toml::parse_error (const lvalue overload).
			[[nodiscard]] explicit operator const parse_error& () const noexcept { return error(); }

			TOML_NODISCARD_CTOR
			explicit parse_result(table&& tbl) noexcept
				: is_err{ false }
			{
				::new (&storage) table{ std::move(tbl) };
			}

			TOML_NODISCARD_CTOR
			explicit parse_result(parse_error&& err) noexcept
				: is_err{ true }
			{
				::new (&storage) parse_error{ std::move(err) };
			}

			/// \brief	Move constructor.
			TOML_NODISCARD_CTOR
			parse_result(parse_result&& res) noexcept
				: is_err{ res.is_err }
			{
				if (is_err)
					::new (&storage) parse_error{ std::move(res).error() };
				else
					::new (&storage) table{ std::move(res).get() };
			}

			/// \brief	Move-assignment operator.
			parse_result& operator=(parse_result&& rhs) noexcept
			{
				if (is_err != rhs.is_err)
				{
					destroy();
					is_err = rhs.is_err;
					if (is_err)
						::new (&storage) parse_error{ std::move(rhs).error() };
					else
						::new (&storage) table{ std::move(rhs).get() };
				}
				else
				{
					if (is_err)
						error() = std::move(rhs).error();
					else
						get() = std::move(rhs).get();
				}
				return *this;
			}

			/// \brief	Destructor.
			~parse_result() noexcept
			{
				destroy();
			}

			/// \brief	Gets a node_view for the selected key-value pair in the wrapped table.
			///
			/// \param 	key The key used for the lookup.
			///
			/// \returns	A view of the value at the given key if parsing was successful and a matching key existed,
			/// 			or an empty node view.
			///
			/// \see toml::node_view
			[[nodiscard]] node_view<node> operator[] (string_view key) noexcept
			{
				return is_err ? node_view<node>{} : get()[key];
			}

			/// \brief	Gets a node_view for the selected key-value pair in the wrapped table (const overload).
			[[nodiscard]] node_view<const node> operator[] (string_view key) const noexcept
			{
				return is_err ? node_view<const node>{} : get()[key];
			}

			/// \brief	Returns an iterator to the first key-value pair in the wrapped table.
			/// \remarks Returns a default-constructed 'nothing' iterator if the parsing failed.
			[[nodiscard]] table_iterator begin() noexcept
			{
				return is_err ? table_iterator{} : get().begin();
			}

			/// \brief	Returns an iterator to the first key-value pair in the wrapped table.
			/// \remarks Returns a default-constructed 'nothing' iterator if the parsing failed.
			[[nodiscard]] const_table_iterator begin() const noexcept
			{
				return is_err ? const_table_iterator{} : get().begin();
			}

			/// \brief	Returns an iterator to the first key-value pair in the wrapped table.
			/// \remarks Returns a default-constructed 'nothing' iterator if the parsing failed.
			[[nodiscard]] const_table_iterator cbegin() const noexcept
			{
				return is_err ? const_table_iterator{} : get().cbegin();
			}

			/// \brief	Returns an iterator to one-past-the-last key-value pair in the wrapped table.
			/// \remarks Returns a default-constructed 'nothing' iterator if the parsing failed.
			[[nodiscard]] table_iterator end() noexcept
			{
				return is_err ? table_iterator{} : get().end();
			}

			/// \brief	Returns an iterator to one-past-the-last key-value pair in the wrapped table.
			/// \remarks Returns a default-constructed 'nothing' iterator if the parsing failed.
			[[nodiscard]] const_table_iterator end() const noexcept
			{
				return is_err ? const_table_iterator{} : get().end();
			}

			/// \brief	Returns an iterator to one-past-the-last key-value pair in the wrapped table.
			/// \remarks Returns a default-constructed 'nothing' iterator if the parsing failed.
			[[nodiscard]] const_table_iterator cend() const noexcept
			{
				return is_err ? const_table_iterator{} : get().cend();
			}
	};

	TOML_ABI_NAMESPACE_END

	#else

	using parse_result = table;

	#endif
}


namespace toml::impl
{
	#if TOML_EXCEPTIONS
		TOML_ABI_NAMESPACE_START(impl_ex)
	#else
		TOML_ABI_NAMESPACE_START(impl_noex)
	#endif

	[[nodiscard]] TOML_API
	parse_result do_parse(utf8_reader_interface&&) TOML_MAY_THROW;

	TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS
}


namespace toml
{
	#if TOML_EXCEPTIONS
		TOML_ABI_NAMESPACE_START(parse_ex)
	#else
		TOML_ABI_NAMESPACE_START(parse_noex)
	#endif

	/// \brief	Parses a TOML document from a string view.
	///
	/// \detail \cpp
	/// auto tbl = toml::parse("a = 3"sv);
	/// std::cout << tbl["a"] << std::endl;
	/// 
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	/// 
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	/// 						If you don't have a path (or you have no intention of using paths in diagnostics)
	/// 						then this parameter can safely be left blank.
	///
	/// \returns <strong><em>With exceptions:</em></strong> A toml::table. <br>
	/// 		 <strong><em>Without exceptions:</em></strong> A toml::parse_result detailing the parsing outcome.
	[[nodiscard]]
	TOML_API
	parse_result parse(std::string_view doc, std::string_view source_path = {}) TOML_MAY_THROW;

	/// \brief	Parses a TOML document from a string view.
	/// 
	/// \detail \cpp
	/// auto tbl = toml::parse("a = 3"sv, "foo.toml");
	/// std::cout << tbl["a"] << std::endl;
	/// 
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	/// 
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	///
	/// \returns <strong><em>With exceptions:</em></strong> A toml::table. <br>
	/// 		 <strong><em>Without exceptions:</em></strong> A toml::parse_result detailing the parsing outcome.
	[[nodiscard]]
	TOML_API
	parse_result parse(std::string_view doc, std::string&& source_path) TOML_MAY_THROW;

	#ifdef __cpp_lib_char8_t

	/// \brief	Parses a TOML document from a char8_t string view.
	///
	/// \detail \cpp
	/// auto tbl = toml::parse(u8"a = 3"sv);
	/// std::cout << tbl["a"] << std::endl;
	/// 
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	/// 
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	/// 						If you don't have a path (or you have no intention of using paths in diagnostics)
	/// 						then this parameter can safely be left blank.
	///
	/// \returns <strong><em>With exceptions:</em></strong> A toml::table. <br>
	/// 		 <strong><em>Without exceptions:</em></strong> A toml::parse_result detailing the parsing outcome.
	/// 
	/// \attention This overload is not available if your compiler does not support char8_t-based strings.
	[[nodiscard]]
	TOML_API
	parse_result parse(std::u8string_view doc, std::string_view source_path = {}) TOML_MAY_THROW;

	/// \brief	Parses a TOML document from a char8_t string view.
	/// 
	/// \detail \cpp
	/// auto tbl = toml::parse(u8"a = 3"sv, "foo.toml");
	/// std::cout << tbl["a"] << std::endl;
	/// 
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	/// 
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	///
	/// \returns <strong><em>With exceptions:</em></strong> A toml::table. <br>
	/// 		 <strong><em>Without exceptions:</em></strong> A toml::parse_result detailing the parsing outcome.
	/// 
	/// \attention This overload is not available if your compiler does not support char8_t-based strings.
	[[nodiscard]]
	TOML_API
	parse_result parse(std::u8string_view doc, std::string&& source_path) TOML_MAY_THROW;

	#endif // __cpp_lib_char8_t

	/// \brief	Parses a TOML document from a stream.
	///
	/// \detail \cpp
	/// std::stringstream ss;
	/// ss << "a = 3"sv;
	/// 
	/// auto tbl = toml::parse(ss);
	/// std::cout << tbl["a"] << std::endl;
	/// 
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	/// 
	/// \tparam	Char			The stream's underlying character type. Must be 1 byte in size.
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	/// 						If you don't have a path (or you have no intention of using paths in diagnostics)
	/// 						then this parameter can safely be left blank.
	///
	/// \returns <strong><em>With exceptions:</em></strong> A toml::table. <br>
	/// 		 <strong><em>Without exceptions:</em></strong> A toml::parse_result detailing the parsing outcome.
	template <typename Char>
	[[nodiscard]]
	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::basic_istream<Char>& doc, std::string_view source_path = {}) TOML_MAY_THROW
	{
		static_assert(
			sizeof(Char) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		return impl::do_parse(impl::utf8_reader{ doc, source_path });
	}

	/// \brief	Parses a TOML document from a stream.
	///
	/// \detail \cpp
	/// std::stringstream ss;
	/// ss << "a = 3"sv;
	/// 
	/// auto tbl = toml::parse(ss, "foo.toml");
	/// std::cout << tbl["a"] << std::endl;
	/// 
	/// \ecpp
	/// 
	/// \out
	/// 3
	/// \eout
	/// 
	/// \tparam	Char			The stream's underlying character type. Must be 1 byte in size.
	/// \param 	doc				The TOML document to parse. Must be valid UTF-8.
	/// \param 	source_path		The path used to initialize each node's `source().path`.
	///
	/// \returns <strong><em>With exceptions:</em></strong> A toml::table. <br>
	/// 		 <strong><em>Without exceptions:</em></strong> A toml::parse_result detailing the parsing outcome.
	template <typename Char>
	[[nodiscard]]
	TOML_EXTERNAL_LINKAGE
	parse_result parse(std::basic_istream<Char>& doc, std::string&& source_path) TOML_MAY_THROW
	{
		static_assert(
			sizeof(Char) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		return impl::do_parse(impl::utf8_reader{ doc, std::move(source_path) });
	}

	/// \brief	Parses a TOML document from a file.
	///
	/// \detail \cpp
	/// #include <fstream>
	/// 
	/// toml::parse_result get_foo_toml()
	/// {
	///		return toml::parse_file("foo.toml");
	/// }
	/// \ecpp
	/// 
	/// \tparam	Char			The path's character type. Must be 1 byte in size.
	/// \param 	file_path		The TOML document to parse. Must be valid UTF-8.
	///
	/// \returns <strong><em>With exceptions:</em></strong> A toml::table. <br>
	/// 		 <strong><em>Without exceptions:</em></strong> A toml::parse_result detailing the parsing outcome.
	/// 
	/// \attention You must `#include <fstream>` to use this function (toml++
	/// 		 does not transitively include it for you).
	template <typename Char>
	[[nodiscard]]
	TOML_EXTERNAL_LINKAGE
	parse_result parse_file(std::basic_string_view<Char> file_path) TOML_MAY_THROW
	{
		static_assert(
			sizeof(Char) == 1,
			"The path's character type must be 1 byte in size."
		);

		auto str = std::string(reinterpret_cast<const char*>(file_path.data()), file_path.length());
		auto ifs = std::basic_ifstream<Char>{ str };
		return parse( ifs, std::move(str) );
	}

	#if !TOML_ALL_INLINE
		extern template TOML_API parse_result parse(std::istream&, std::string_view) TOML_MAY_THROW;
		extern template TOML_API parse_result parse(std::istream&, std::string&&) TOML_MAY_THROW;
		extern template TOML_API parse_result parse_file(std::string_view) TOML_MAY_THROW;
		#ifdef __cpp_lib_char8_t
			extern template TOML_API parse_result parse_file(std::u8string_view) TOML_MAY_THROW;
		#endif
	#endif

	// Q: "why are the parse_file functions templated??"
	// A: I don't want to force users to drag in <fstream> if they're not going to do
	//    any parsing directly from files. Keeping them templated delays their instantiation
	//    until they're actually required, so only those users wanting to use parse_file()
	//    are burdened by the <fstream> overhead.

	template <typename Char>
	[[nodiscard]]
	inline parse_result parse_file(const std::basic_string<Char>& file_path) TOML_MAY_THROW
	{
		return parse_file(std::basic_string_view<Char>{ file_path });
	}

	template <typename Char>
	[[nodiscard]]
	inline parse_result parse_file(const Char* file_path) TOML_MAY_THROW
	{
		return parse_file(std::basic_string_view<Char>{ file_path });
	}

	TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS

	/// \brief	Convenience literal operators for working with TOML++.
	/// 
	/// \detail This namespace exists so you can safely hoist the UDL operators into another scope
	/// 		 without dragging in everything in the toml namespace: \cpp
	/// 
	/// #include <toml++/toml.h>
	///	using namespace toml::literals;
	///
	///	int main()
	///	{
	///		auto tbl = "vals = [1, 2, 3]"_toml;
	///
	///		// ... do stuff with the table generated by the "_toml" UDL ...
	///
	///		return 0;
	///	}
	/// \ecpp
	///
	inline namespace literals
	{
		#if TOML_EXCEPTIONS
			TOML_ABI_NAMESPACE_START(lit_ex)
		#else
			TOML_ABI_NAMESPACE_START(lit_noex)
		#endif

		/// \brief	Parses TOML data from a string.
		/// 
		/// \detail \cpp
		/// using namespace toml::literals;
		/// 
		/// auto tbl = "a = 3"_toml;
		/// std::cout << tbl["a"] << std::endl;
		/// 
		/// \ecpp
		/// 
		/// \out
		/// 3
		/// \eout
		/// 
		/// \param 	str	The string data.
		/// \param 	len	The string length.
		///
		/// \returns <strong><em>With exceptions:</em></strong> A toml::table. <br>
		/// 		 <strong><em>Without exceptions:</em></strong> A toml::parse_result detailing the parsing outcome.
		[[nodiscard]]
		TOML_API
		parse_result operator"" _toml(const char* str, size_t len) TOML_MAY_THROW;

		#ifdef __cpp_lib_char8_t

		/// \brief	Parses TOML data from a string.
		/// 
		/// \detail \cpp
		/// using namespace toml::literals;
		/// 
		/// auto tbl = u8"a = 3"_toml;
		/// std::cout << tbl["a"] << std::endl;
		/// 
		/// \ecpp
		/// 
		/// \out
		/// 3
		/// \eout
		/// 
		/// \param 	str	The string data.
		/// \param 	len	The string length.
		///
		/// \returns <strong><em>With exceptions:</em></strong> A toml::table. <br>
		/// 		 <strong><em>Without exceptions:</em></strong> A toml::parse_result detailing the parsing outcome.
		/// 
		/// \attention This overload is not available if your compiler does not support char8_t-based strings.
		[[nodiscard]]
		TOML_API
		parse_result operator"" _toml(const char8_t* str, size_t len) TOML_MAY_THROW;

		#endif // __cpp_lib_char8_t

		TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS
	}
}

TOML_POP_WARNINGS // TOML_DISABLE_PADDING_WARNINGS
