#include "tests.h"

TEST_CASE("parsing - key-value pairs")
{
	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(
key = "value"
bare_key = "value"
bare-key = "value"
1234 = "value"
"" = "blank"
)"sv),
		[](table&& tbl)
		{
			CHECK(tbl.size() == 5);
			CHECK(tbl[S("key")] == S("value"sv));
			CHECK(tbl[S("bare_key")] == S("value"sv));
			CHECK(tbl[S("bare-key")] == S("value"sv));
			CHECK(tbl[S("1234")] == S("value"sv));
			CHECK(tbl[S("")] == S("blank"sv));
		}
	);

	parsing_should_fail(FILE_LINE_ARGS, S(R"(key = # INVALID)"sv));

	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(
"127.0.0.1" = "value"
"character encoding" = "value"
"ʎǝʞ" = "value"
'key2' = "value"
'quoted "value"' = "value"
'' = 'blank'
)"sv),
		[](table&& tbl)
		{
			CHECK(tbl[S("127.0.0.1")] == S("value"sv));
			CHECK(tbl[S("character encoding")] == S("value"sv));
			CHECK(tbl[S("ʎǝʞ")] == S("value"sv));
			CHECK(tbl[S("key2")] == S("value"sv));
			CHECK(tbl[S("quoted \"value\"")] == S("value"sv));
			CHECK(tbl[S("")] == S("blank"sv));
		}
	);

	parsing_should_fail(FILE_LINE_ARGS, S(R"(= "no key name")"sv));

	parsing_should_fail(FILE_LINE_ARGS, S(R"(
# DO NOT DO THIS
name = "Tom"
name = "Pradyun"
)"sv));

}

TEST_CASE("parsing - key-value pairs (dotted)")
{
	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(
name = "Orange"
physical.color = "orange"
physical.shape = "round"
site."google.com" = true
3.14159 = "pi"
)"sv),
		[](table&& tbl)
		{
			CHECK(tbl.size() == 4);
			CHECK(tbl[S("name")] == S("Orange"sv));
			CHECK(tbl[S("physical")][S("color")] == S("orange"sv));
			CHECK(tbl[S("physical")][S("shape")] == S("round"sv));
			CHECK(tbl[S("site")][S("google.com")] == true);
			CHECK(tbl[S("3")][S("14159")] == S("pi"sv));
		}
	);


	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(
fruit.apple.smooth = true
fruit.orange = 2
)"sv),
		[](table&& tbl)
		{
			CHECK(tbl[S("fruit")][S("apple")][S("smooth")] == true);
			CHECK(tbl[S("fruit")][S("orange")] == 2);
		}
	);

	parsing_should_fail(FILE_LINE_ARGS, S(R"(
# THIS IS INVALID
fruit.apple = 1
fruit.apple.smooth = true
)"sv));

	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(
# VALID BUT DISCOURAGED

apple.type = "fruit"
orange.type = "fruit"

apple.skin = "thin"
orange.skin = "thick"

apple.color = "red"
orange.color = "orange"
)"sv),
		[](table&& tbl)
		{
			CHECK(tbl[S("apple")][S("type")] == S("fruit"sv));
			CHECK(tbl[S("apple")][S("skin")] == S("thin"sv));
			CHECK(tbl[S("apple")][S("color")] == S("red"sv));
			CHECK(tbl[S("orange")][S("type")] == S("fruit"sv));
			CHECK(tbl[S("orange")][S("skin")] == S("thick"sv));
			CHECK(tbl[S("orange")][S("color")] == S("orange"sv));
		}
	);

	parsing_should_succeed(
		FILE_LINE_ARGS,
		S(R"(
# RECOMMENDED

apple.type = "fruit"
apple.skin = "thin"
apple.color = "red"

orange.type = "fruit"
orange.skin = "thick"
orange.color = "orange"
)"sv),
		[](table&& tbl)
		{
			CHECK(tbl[S("apple")][S("type")] == S("fruit"sv));
			CHECK(tbl[S("apple")][S("skin")] == S("thin"sv));
			CHECK(tbl[S("apple")][S("color")] == S("red"sv));
			CHECK(tbl[S("orange")][S("type")] == S("fruit"sv));
			CHECK(tbl[S("orange")][S("skin")] == S("thick"sv));
			CHECK(tbl[S("orange")][S("color")] == S("orange"sv));
		}
	);

	// toml/issues/644 ('+' in bare keys) & toml/issues/687 (unicode bare keys)
	#if TOML_LANG_UNRELEASED
		parsing_should_succeed(
			FILE_LINE_ARGS,
			S(R"(
				key+1 = 0
				ʎǝʞ2 = 0
			)"sv),
			[](table&& tbl)
			{
				CHECK(tbl.size() == 2);
				CHECK(tbl[S("key+1")] == 0);
				CHECK(tbl[S("ʎǝʞ2")] == 0);
			}
		);
	#else
		parsing_should_fail(FILE_LINE_ARGS, R"(key+1 = 0)"sv);
		parsing_should_fail(FILE_LINE_ARGS, R"(ʎǝʞ2 = 0)"sv);
	#endif
}
