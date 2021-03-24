//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      base/utils/CSV.cpp
//! @brief     Implements class CSV
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cassert>

#include "base/utils/CSV.h"

namespace nsx {

CSV::CSV(char delim, char quotchar) : _delim(delim), _quotchar(quotchar) { }

std::vector<std::string> CSV::getRow(std::istream& stream)
{
    char delim(0);
    const char eof = std::char_traits<char>::eof();
    std::vector<std::string> row;

    while (delim != '\n' && delim != eof)
        row.push_back(getToken(stream, delim));

    return row;
}

std::string CSV::getToken(std::istream& stream, char& delim) const
{
    std::string tok;
    char curr, eof, n1, n2;
    bool in_string(false);
    bool done(false);
    eof = std::char_traits<char>::eof();

    int pos = stream.tellg();
    stream.seekg(0, std::ios_base::end);
    int size = stream.tellg();
    stream.seekg(pos - size, std::ios_base::end);
    assert(pos == stream.tellg());

    while (!done && !stream.eof()) {
        curr = eof;
        stream.get(curr);

        if (curr == eof) {
            done = true;
            continue;
        }

        if (!in_string) {
            if ((curr == _delim) || (curr == '\n')) {
                done = true;
                continue;
            }
            if (curr == _quotchar)
                in_string = true;
        } else {
            // check whether we are at end of string, or a quoted '"'
            if (curr == _quotchar) {
                pos = stream.tellg();

                // reached end of file
                if (size - pos < 2)
                    in_string = false;
                // else check for triple quote
                else {
                    stream.get(n1);
                    stream.get(n2);

                    if ((n1 == _quotchar) && (n2 == _quotchar)) {
                        tok += _quotchar;
                        tok += _quotchar;
                        tok += _quotchar;
                        continue;
                    }
                    in_string = false;
                    stream.seekg(pos);
                    assert(pos == stream.tellg());
                }
            }
        }

        tok += curr;
    }

    if (stream.eof())
        curr = eof;

    delim = curr; // write this so caller knows if token ended by delim, newline,
                  // or eof

    // std::cout << "####" << tok << "####" << (int)delim << std::endl;

    return tok;
}

} // namespace nsx
