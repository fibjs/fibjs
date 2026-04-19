#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <string_view>
#include "parse.h"

namespace fibjs {
namespace dotenv_parser {

using store_t = std::map<std::string, std::string>;

inline std::string_view trim_spaces(std::string_view input)
{
    if (input.empty())
        return "";

    auto pos_start = input.find_first_not_of(" \t\n");
    if (pos_start == std::string_view::npos)
        return "";

    auto pos_end = input.find_last_not_of(" \t\n");
    if (pos_end == std::string_view::npos)
        return input.substr(pos_start);

    return input.substr(pos_start, pos_end - pos_start + 1);
}

inline void skip_inline_spaces(_parser& parser)
{
    while (!parser.end()) {
        char ch = parser.get();
        if (ch == ' ' || ch == '\t')
            parser.skip();
        else
            break;
    }
}

inline void skip_line(_parser& parser)
{
    parser.skipUntil('\n');
    if (parser.get() == '\n')
        parser.skip();
}

inline void unescape_double_quoted_newlines(std::string& value)
{
    size_t pos = 0;

    while ((pos = value.find("\\n", pos)) != std::string::npos) {
        value.replace(pos, 2, "\n");
        pos += 1;
    }
}

inline void skip_to_next_entry(_parser& parser)
{
    if (parser.get() == '\n')
        parser.skip();

    parser.skipSpace();
}

inline std::string_view normalize_key(std::string_view key)
{
    key = trim_spaces(key);

    if (key.starts_with("export ")) {
        key.remove_prefix(7);
        key = trim_spaces(key);
    }

    return key;
}

inline void store_plain_value(_parser& parser, std::string_view key, store_t& store)
{
    int32_t value_start = parser.pos;
    parser.skipUntil('\n');

    std::string_view value(parser.string + value_start, parser.pos - value_start);
    size_t hash_character = value.find('#');
    if (hash_character != std::string_view::npos)
        value = value.substr(0, hash_character);

    store.insert_or_assign(std::string(key), std::string(trim_spaces(value)));
    skip_to_next_entry(parser);
}

inline void store_quoted_value(_parser& parser, std::string_view key, store_t& store)
{
    char quote = parser.get();
    parser.skip();

    int32_t value_start = parser.pos;
    parser.skipUntil(quote);

    if (parser.get() == quote) {
        std::string value(parser.string + value_start, parser.pos - value_start);
        if (quote == '"')
            unescape_double_quoted_newlines(value);

        store.insert_or_assign(std::string(key), value);
        parser.skip();
        skip_line(parser);
        parser.skipSpace();
        return;
    }

    parser.pos = value_start - 1;
    int32_t raw_start = parser.pos;
    parser.skipUntil('\n');
    store.insert_or_assign(std::string(key), std::string(parser.string + raw_start, parser.pos - raw_start));
    skip_to_next_entry(parser);
}

inline void parse_content(std::string_view input, store_t& store)
{
    std::string lines(input);
    lines.erase(std::remove(lines.begin(), lines.end(), '\r'), lines.end());

    _parser parser(lines.c_str(), (int32_t)lines.length());
    parser.skipSpace();

    while (!parser.end()) {
        if (parser.get() == '#') {
            skip_line(parser);
            parser.skipSpace();
            continue;
        }

        int32_t key_start = parser.pos;
        parser.skipUntil('=', '\n');

        char separator = parser.get();
        if (separator == 0)
            break;

        if (separator == '\n') {
            parser.skip();
            parser.skipSpace();
            continue;
        }

        std::string_view key(parser.string + key_start, parser.pos - key_start);
        key = normalize_key(key);
        parser.skip();

        if (key.empty()) {
            skip_line(parser);
            parser.skipSpace();
            continue;
        }

        skip_inline_spaces(parser);

        if (parser.end() || parser.get() == '\n') {
            store.insert_or_assign(std::string(key), "");
            skip_to_next_entry(parser);
            continue;
        }

        char ch = parser.get();
        if (ch == '\'' || ch == '"' || ch == '`') {
            store_quoted_value(parser, key, store);
            continue;
        }

        store_plain_value(parser, key, store);
    }
}

} // namespace dotenv_parser
} // namespace fibjs