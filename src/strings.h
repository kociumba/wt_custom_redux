#ifndef STRINGS_H
#define STRINGS_H

#include <string>
#include <string_view>

inline constexpr std::string double_quotes(std::string_view input) {
    std::string result;
    result.reserve(input.size());

    for (const char c : input) {
        if (c == '"') { result += '"'; }
        result += c;
    }

    return result;
}

#endif /* STRINGS_H */
