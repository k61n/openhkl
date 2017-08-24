#pragma once

#include <string>

namespace nsx {

std::string trim(const std::string& input_string);

std::string compress(const std::string& input_string);

std::string join(const std::vector<std::string>& tokens, const char* delimiter);

std::vector<std::string> split(std::string input_string, const char* delimiter);

} // end namespace nsx
