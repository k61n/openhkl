#include <algorithm>
#include <cctype>
#include <cstring>
#include <iterator>
#include <sstream>
#include <vector>

#include "StringIO.h"

namespace nsx
{

std::string trim(const std::string& input_string)
{
    std::string output_string(input_string);
    output_string.erase(output_string.find_last_not_of(" \n\r\t")+1);
    output_string.erase(0,output_string.find_first_not_of(" \n\r\t"));

    return output_string;
}

std::string compress(const std::string& input_string)
{
    std::string output_string(input_string);

    auto check_white_space = [](unsigned char const c) {return std::isspace(c);};
    std::replace_if(output_string.begin(),output_string.end(),check_white_space,' ');

    auto check_multi_space = [](unsigned char const c1,unsigned char const c2) {return std::isspace(c1) && std::isspace(c2);};
    output_string.erase(std::unique(output_string.begin(), output_string.end(), check_multi_space), output_string.end());

    return output_string;
}

std::string join(const std::vector<std::string>& tokens, const char* delimiter)
{
    if (tokens.empty())
        return "";

    std::ostringstream ss;
    std::copy(tokens.begin(), tokens.end()-1, std::ostream_iterator<std::string>(ss, " "));
    ss << tokens.back();

    return ss.str();
}

std::vector<std::string> split(std::string input_string, const char* delimiter)
{
    std::vector<std::string> tokens;

    auto remove_spaces = [](char x){return std::isspace(x);};
    input_string.erase(std::remove_if(input_string.begin(),input_string.end(),remove_spaces),input_string.end());
    auto text_cstr = const_cast<char*>(input_string.c_str());

    char* match = strtok(text_cstr,delimiter);
    while (match) {
        tokens.push_back(match);
        match = strtok(nullptr,delimiter);
    }

    return tokens;
}

} // end namespace nsx
