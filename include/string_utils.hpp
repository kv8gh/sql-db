#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <string>
#include <algorithm>
#include <cctype>

inline std::string trim(const std::string& str){
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";

    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

inline std::string toLower(const std::string& str){
    std::string result = str;
    std::transform(result.begin(),result.end(),result.begin(),[](unsigned char c){
        return std::tolower(c);
    });
    return result;
}

#endif