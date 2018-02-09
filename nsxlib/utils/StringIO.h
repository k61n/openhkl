#pragma once

#include <string>
#include <vector>

namespace nsx {

std::string clear_spaces(const std::string& input_string);

std::string trim(const std::string& input_string);

std::string compress(const std::string& input_string);

std::string join(const std::vector<std::string>& tokens, std::string delimiter = " ");

std::vector<std::string> split(std::string input_string, std::string delimiter = " ");

} // end namespace nsx
