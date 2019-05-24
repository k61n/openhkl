#ifndef CORE_UTILS_CSV_H
#define CORE_UTILS_CSV_H

#include <iostream>
#include <string>
#include <vector>

namespace nsx {

//! @class CSV
//! @brief Simple CSV parser
class CSV {
public:
    CSV(char delim = ',', char quotchar = '"');
    ~CSV();

    std::vector<std::string> getRow(std::istream& stream);

private:
    char _delim;
    char _quotchar;

    std::string getToken(
        std::istream& stream,
        char& delim); //!<< Get next token, write delimeter type
};

} // end namespace nsx

#endif // CORE_UTILS_CSV_H
