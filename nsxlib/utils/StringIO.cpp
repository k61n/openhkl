#include <algorithm>
#include <cctype>

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

} // end namespace nsx
