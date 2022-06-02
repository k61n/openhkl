//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/JonesSymbolParser.cpp
//! @brief     Implements function parseJonesSymbol
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <map>
#include <regex>
#include <stdexcept>
#include <string>

#include "base/utils/StringIO.h"
#include "tables/crystal/JonesSymbolParser.h"

namespace nsx {

Eigen::Transform<double, 3, Eigen::Affine> parseJonesSymbol(const std::string& jonesSymbol)
{
    Eigen::Transform<double, 3, Eigen::Affine> matrix;
    matrix.affine().setZero();

    // Convenient containers to translate directly for some matched symbol to
    // their real counterpart
    std::map<std::string, Eigen::RowVector3d> axis(
        {{"x", {1, 0, 0}}, {"y", {0, 1, 0}}, {"z", {0, 0, 1}}});
    std::map<std::string, double> sign({{"+", 1.0}, {"-", -1.0}});

    // Split the Jones symbol according to ",". This must provide 3 symbols one for each axis
    auto symbols = split(jonesSymbol, ",");
    if (symbols.size() != 3)
        throw std::runtime_error("Invalid Jones-Faithful symbol");

    // The regex for the rotational part (e.g. 2x-y+4z)
    std::regex rotation_re("(?:(?:([+-])(\\d*))([xyz]))");

    // The regex for the translational part (e.g. +1/2,-1/3)
    std::regex translation_re("(?:(?:([+-]\\d+))(?:/(\\d+))?)");

    // Lambda for removing any space for the symbols
    auto check_white_space = [](unsigned char const c) { return std::isspace(c); };

    // The symbols size must be 3. Hence this loop loops over the xyz axis.
    for (size_t i = 0; i < symbols.size(); ++i) {
        // Remove any space fro the current symbol
        auto symbol = symbols[i];
        auto sit = std::remove_if(symbol.begin(), symbol.end(), check_white_space);
        symbol.erase(sit, symbol.end());
        if (symbol.empty())
            throw std::runtime_error("Invalid Jones-Faithful symbol");

        // If the first character of the symbol is either a number or a letter (xyz)
        // then add a + for having a consistent regex for the rotational part
        if (std::isalnum(symbol[0]))
            symbol.insert(0, "+");

        // Find all the matches for the rotational part
        auto rotation_expr_begin = std::sregex_iterator(
            symbol.begin(), symbol.end(), rotation_re, std::regex_constants::match_continuous);
        auto rotation_expr_end = std::sregex_iterator();

        // No matches found, throws
        if (rotation_expr_begin == rotation_expr_end)
            throw std::runtime_error(
                "Invalid Jones-Faithful symbol: could not parse the rotation part");

        // Will store the current match for the rotational part (e.g. +3x)
        // rotation_match[1] stores "+" or "-"
        // rotation_match[2] stores the number
        std::smatch rotation_match;
        for (auto m_it = rotation_expr_begin; m_it != rotation_expr_end; ++m_it) {
            rotation_match = *m_it;

            double factor = sign[rotation_match[1].str()];
            if (rotation_match[2].length() != 0)
                factor *= std::atof(rotation_match[2].str().c_str());
            // Update the column with the current rotation match
            matrix.linear().row(i) += factor * axis[rotation_match[3].str()];
        }

        std::string suffix = rotation_match.suffix().str();

        // The remainder of the symbol after having matched the rotational part must
        // be the translationa part
        if (!suffix.empty()) {
            // The translational match. Must ne of the form +/-n or +/-n/d (e.g. +3,
            // -1/2) translation_match[1] stores the numerator translation_match[2]
            // stores the denominator (if any)
            std::smatch translation_match;
            bool success = std::regex_match(suffix, translation_match, translation_re);
            // Could not match the translationa part, throws
            if (!success)
                throw std::runtime_error("Invalid Jones-Faithful symbol: could not "
                                         "parse the translation part");

            double val = std::atof(translation_match[1].str().c_str());
            // Check that a denominator has been provided
            if (translation_match[2].length() > 0) {
                int den = std::atoi(translation_match[2].str().c_str());
                // Check that the denominator is not 0
                if (den == 0) {
                    throw std::runtime_error(
                        "Invalid Jones-Faithful symbol: ill-defined translation part");
                }
                val /= den;
            }
            // Updates the translationa part
            matrix.translation()(i) = val;
        }
    }

    return matrix;
}

} // namespace nsx
