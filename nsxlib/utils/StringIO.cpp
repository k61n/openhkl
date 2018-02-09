#include <algorithm>
#include <cctype>
#include <cstring>
#include <iterator>
#include <sstream>
#include <vector>

#include "StringIO.h"

namespace nsx {

std::string trim(const std::string& input_string)
{
    auto output_string(input_string);

    size_t pos;

    pos = output_string.find_last_not_of(" \n\r\t");
    if (pos != std::string::npos) {
        output_string.erase(pos+1);
    }

    pos = output_string.find_first_not_of(" \n\r\t");
    if (pos != std::string::npos) {
        output_string.erase(0,pos);
    }

    return output_string;
}

std::string clear_spaces(const std::string& input_string) {

    auto output_string(input_string);

    auto check_white_space = [](unsigned char const c) {return std::isspace(c);};
    auto sit = std::remove_if(output_string.begin(), output_string.end(), check_white_space);
    output_string.erase(sit,output_string.end());

    return output_string;
}

std::string compress(const std::string& input_string)
{
    std::string output_string = trim(input_string);

    auto check_white_space = [](unsigned char const c) {return std::isspace(c);};
    std::replace_if(output_string.begin(),output_string.end(),check_white_space,' ');

    auto check_multi_space = [](unsigned char const c1,unsigned char const c2) {return std::isspace(c1) && std::isspace(c2);};
    output_string.erase(std::unique(output_string.begin(), output_string.end(), check_multi_space), output_string.end());

    return output_string;
}

std::string join(const std::vector<std::string>& tokens, std::string delimiter)
{
    if (tokens.empty()) {
        return "";
    }

    std::ostringstream ss;
    std::copy(tokens.begin(), tokens.end()-1, std::ostream_iterator<std::string>(ss,delimiter.c_str()));
    ss << tokens.back();

    return ss.str();
}

std::vector<std::string> split(std::string input_string, std::string delimiter)
{
    std::vector<std::string> tokens;

    auto text_cstr = const_cast<char*>(input_string.c_str());

    char* match = strtok(text_cstr,delimiter.c_str());
    while (match) {
        tokens.push_back(trim(match));
        match = strtok(nullptr,delimiter.c_str());
    }

    return tokens;
}

} // end namespace nsx
