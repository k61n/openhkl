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

#include <algorithm>
#include <stdexcept>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "../crystal/SpaceGroup.h"
#include "../crystal/SpaceGroupSymbols.h"
#include "../crystal/Peak3D.h"

namespace nsx {

SpaceGroup::SpaceGroup(const std::string& symbol)
{
    // Get the generators of the input space group.
    SpaceGroupSymbols* sg = SpaceGroupSymbols::Instance();

    // Get a reduced version of the spacegroup symbol
    _symbol = sg->getReducedSymbol(symbol);
    if (!sg->getGenerators(_symbol,_generators)) {
        throw std::runtime_error("Unknown space group: " + _symbol + "(" + symbol + ")");
    }
    generateGroupElements();
}

SpaceGroup::SpaceGroup(std::string symbol, std::string generators):
    _symbol(std::move(symbol)),
    _generators(std::move(generators))
{
    SpaceGroupSymbols* sg = SpaceGroupSymbols::Instance();
    if (sg->getGenerators(_symbol,_generators)) {
        throw std::runtime_error("Space Group already registered");
    }
    sg->addSpaceGroup(_symbol,_generators);
    generateGroupElements();
}

SpaceGroup& SpaceGroup::operator=(const SpaceGroup& other)
{
    if (this != &other) {
        _symbol = other._symbol;
        _generators = other._generators;
        _groupElements = other._groupElements;
    }
    return *this;
}

char SpaceGroup::getBravaisType() const
{
    std::vector<int> nrot(13,0);
    int nPureTrans(0);
    const int isCentro = isCentrosymmetric() ? 2 : 1;

    for (auto&& g : _groupElements) {
        assert(g.getAxisOrder()+6 >= 0);
        size_t idx = size_t(g.getAxisOrder()+6);
        ++nrot[idx];
        if (g.isPureTranslation()) {
            nPureTrans++;
        }
    }
    const int fact = (1+nPureTrans)*isCentro;

    // Cubic
    if ((nrot[3] + nrot[9]) == 8*fact) {
       return 'c';
    }
    // Hexagonal
    if ((nrot[0] + nrot[12]) == 2*fact) {
       return 'h';
    }
    // Trigonal
    if ((nrot[3] + nrot[9]) == 2*fact) {
        return 'h';
    }
    // Tetragonal
    if ((nrot[2] + nrot[10]) == 2*fact) {
        return 't';
    }
    // Orthorhombic
    if ((nrot[4] + nrot[8]) == 3*fact) {
        return 'o';
    }
    // Monoclinic
    if ((nrot[4] + nrot[8]) == fact) {
        return 'm';
    }
    // Triclinic, only remaining case
    return 'a';
}

double SpaceGroup::fractionExtinct(std::vector<std::array<double, 3> > hkl)
{
    unsigned int extinct = 0;
    unsigned int total = hkl.size();

    for (auto&& i: hkl) {
        if (isExtinct(i[0],i[1], i[2])) {
            ++extinct;
        }
    }
    return double(extinct) / double(total);
}

std::string SpaceGroup::getBravaisTypeSymbol() const
{
    std::string bravais;
    bravais += getBravaisType();
    bravais += getSymbol()[0];
    return bravais;
}

int SpaceGroup::getID() const
{
    SpaceGroupSymbols* sg = SpaceGroupSymbols::Instance();
    auto&& full_symbol = sg->getFullSymbol(_symbol);
    return sg->getID(full_symbol);
}

std::vector<PeakList> SpaceGroup::findEquivalences(const PeakList &peak_list, bool friedel) const
{
    std::vector<PeakList> peak_equivs;

    for (auto&& peak: peak_list ) {
        bool found_equivalence = false;
        int h1, h2, k1, k2, l1, l2;
        Eigen::RowVector3i hkl = peak->getIntegerMillerIndices();

        h1 = hkl[0];
        k1 = hkl[1];
        l1 = hkl[2];

        for (size_t i = 0; i < peak_equivs.size() && !found_equivalence; ++i) {
            hkl = peak_equivs[i][0]->getIntegerMillerIndices();
            h2 = hkl[0];
            k2 = hkl[1];
            l2 = hkl[2];

            if ( (friedel && isFriedelEquivalent(h1, k1, l1, h2, k2, l2))
                 || (!friedel && isEquivalent(h1, k1, l1, h2, k2, l2))) {
                found_equivalence = true;
                peak_equivs[i].push_back(peak);
                continue;
            }
        }

        // didn't find an equivalence?
        if ( !found_equivalence) {
            peak_equivs.emplace_back(PeakList{peak});
        }
    }
    return peak_equivs;
}

bool SpaceGroup::isCentrosymmetric() const
{
    for (auto&& g : _groupElements) {
        if (g.getAxisOrder() == -1) {
            return true;
        }
    }
    return false;
}

const std::string& SpaceGroup::getSymbol() const
{
    return _symbol;
}

const std::string& SpaceGroup::getGenerators() const
{
    return _generators;
}

const SymOpList& SpaceGroup::getGroupElements() const
{
    return _groupElements;
}

void SpaceGroup::generateGroupElements()
{
    _groupElements.clear();
    SymOpList generators;
    std::vector<std::string> gens;
    boost::split(gens, _generators, boost::is_any_of(";"));
    generators.reserve(gens.size()+1);
    generators.emplace_back(SymOp(affineTransformation::Identity()));

    for (auto&& g : gens) {
        auto&& gen = SymOp(g);
        generators.emplace_back(gen);
    }

    _groupElements.emplace_back(SymOp(affineTransformation::Identity()));
    unsigned int oldSize = 0;

    while (oldSize != _groupElements.size()) {
        oldSize = _groupElements.size();
        for (unsigned int i = 0; i < _groupElements.size(); ++i) {
            for (auto&& g : generators) {
                auto newElement = _groupElements[i]*g;
                auto it = std::find(_groupElements.begin(), _groupElements.end(), newElement);
                if (it == _groupElements.end()) {
                    _groupElements.push_back(newElement);
                }
            }
        }
    }
}

bool SpaceGroup::isExtinct(double h, double k, double l) const
{
    // todo(jonathan): improve this routine? need a journal reference
    Eigen::Vector3d hkl(h,k,l);
    for (auto&& element : _groupElements) {
        if (element.hasTranslation()) {
            Eigen::Vector3d t = element.getTranslationPart();
            double scalar = t.dot(hkl);
            std::complex<double> prefactor = 1.0+std::exp(std::complex<double>(0,2*M_PI)*scalar);
            if (std::abs(prefactor)<1e-3) {
                //if (std::abs(std::remainder(scalar,1.0))>1e-3)
                Eigen::Vector3d rhkl = element.getRotationPart()*hkl;
                if (std::abs(rhkl(0)-hkl(0))<1e-3 && std::abs(rhkl(1)-hkl(1))<1e-3 && std::abs(rhkl(2)-hkl(2))<1e-3) {
                    return true;
                }
            }
        }
    }
    return false;
}

void SpaceGroup::print(std::ostream& os) const
{
    os << "Symmetry elements of space group "<< _symbol << std::endl;
    for (auto&& g: _groupElements) {
        os << g << " ; ";
    }
    os << std::endl;
}

std::ostream& operator<<(std::ostream& os, const SpaceGroup& sg)
{
    sg.print(os);
    return os;
}

bool SpaceGroup::isEquivalent(double h1, double k1, double l1, double h2, double k2, double l2, bool friedel) const
{
    return isEquivalent(Eigen::Vector3d(h1, k1, l1), Eigen::Vector3d(h2, k2, l2), friedel);
}

bool SpaceGroup::isEquivalent(const Eigen::Vector3d& a, const Eigen::Vector3d& b, bool friedel) const
{
    const auto& elements = getGroupElements();
    const double eps = 1e-6;

    for (auto&& element : elements) {
        // todo(jonathan): check that this edit is correct!
        const auto rotation = element.getRotationPart();
        const auto rotated = rotation * a;

        if (std::max((rotated-b).maxCoeff(), (b-rotated).maxCoeff()) < eps) {
            return true;
        }

        if (friedel && std::max((rotated+b).maxCoeff(), (-b-rotated).maxCoeff()) < eps) {
            return true;
        }
    }
    return false;
}

bool SpaceGroup::isFriedelEquivalent(double h1, double k1, double l1, double h2, double k2, double l2) const
{
    const auto& elements = getGroupElements();
    Eigen::Vector3d rotated;
    for (const auto& element : elements) {
        // todo(jonathan): check that this edit is correct!
        //rotated = element.getMatrix()*Eigen::Vector3d(h1,k1,l1);
        rotated = element.getRotationPart()*Eigen::Vector3d(h1,k1,l1);

        if (std::abs(rotated[0]-h2)<1e-6 && std::abs(rotated[1]-k2)<1e-6 && std::abs(rotated[2]-l2)<1e-6) {
            return true;
        }
        // compare against Friedel reflection
        if (std::abs(rotated[0]+h2)<1e-6 && std::abs(rotated[1]+k2)<1e-6 && std::abs(rotated[2]+l2)<1e-6) {
            return true;
        }
    }
    return false;
}

} // end namespace nsx
