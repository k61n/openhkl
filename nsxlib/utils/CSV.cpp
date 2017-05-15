/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher
 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "CSV.h"

#include <cassert>

namespace nsx {

CSV::CSV(char delim, char quotchar): _delim(delim), _quotchar(quotchar)
{
}

CSV::~CSV()
{

}

std::vector<std::string> CSV::getRow(std::istream &stream)
{
    char delim(0);
    const char eof = std::char_traits<char>::eof();
    std::vector<std::string> row;

    while(delim != '\n' && delim != eof)
        row.push_back(getToken(stream, delim));

    return row;
}

std::string CSV::getToken(std::istream &stream, char &delim)
{
    std::string tok("");
    char curr, prev, eof, n1, n2;
    bool in_string(false);
    bool done(false);
    eof = std::char_traits<char>::eof();

    int pos = stream.tellg();
    stream.seekg(0, std::ios_base::end);
    int size = stream.tellg();
    stream.seekg(pos-size, std::ios_base::end);
    assert(pos == stream.tellg());

    while(!done && !stream.eof()) {
        curr = eof;
        stream.get(curr);

        if (curr == eof) {
            done = true;
            continue;
        }

        if (!in_string) {
            if ( (curr == _delim) || (curr == '\n')) {
                done = true;
                continue;
            }
            if (curr == _quotchar)
                in_string = true;
        }
        else {
            // check whether we are at end of string, or a quoted '"'
            if ( curr == _quotchar) {
                pos = stream.tellg();

                // reached end of file
                if (size-pos < 2) {
                    in_string = false;
                }
                // else check for triple quote
                else {
                    stream.get(n1);
                    stream.get(n2);

                    if ( (n1 == _quotchar) && (n2 == _quotchar)) {
                        tok += _quotchar;
                        tok += _quotchar;
                        tok += _quotchar;
                        prev = curr;
                        continue;
                    } else {
                        in_string = false;
                        stream.seekg(pos);
                        assert(pos == stream.tellg());
                    }
                }
            }
        }

        tok += curr;
        prev = curr;
    }

    if (stream.eof())
        curr = eof;


    delim = curr; // write this so caller knows if token ended by delim, newline, or eof

    // std::cout << "####" << tok << "####" << (int)delim << std::endl;

    return tok;
}

} // end namespace nsx

