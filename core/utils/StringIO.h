#ifndef CORE_UTILS_STRINGIO_H
#define CORE_UTILS_STRINGIO_H

#include <string>
#include <vector>

namespace nsx {

std::string clear_spaces(const std::string& input_string);

std::string trim(const std::string& input_string);

std::string compress(const std::string& input_string);

std::string join(const std::vector<std::string>& tokens, std::string delimiter = " ");

std::vector<std::string> split(const std::string& input_string, const std::string& delimiter = " ");

} // end namespace nsx

#endif // CORE_UTILS_STRINGIO_H
