#pragma once

#include <string>
#include <vector>

namespace nsx {

void removed_spaces(std::string& input_string);

std::string trimmed(const std::string& input_string);

void trim(std::string& input_string);

std::string compress(const std::string& input_string);

std::string join(const std::vector<std::string>& tokens, const char* delimiter);

std::vector<std::string> split(std::string input_string, const char* delimiter);

} // end namespace nsx
