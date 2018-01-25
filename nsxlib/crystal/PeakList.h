/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Institut Laue-Langevin
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

#pragma once

#include <initializer_list>
#include <vector>

#include "CrystalTypes.h"

namespace nsx {

class PeakList {

public:

    using peak_list = std::vector<sptrPeak3D>;
    using iterator = peak_list::iterator;
    using const_iterator = peak_list::const_iterator;
    using value_type = sptrPeak3D;

    PeakList()=default;

    PeakList(const std::initializer_list<sptrPeak3D>& peaks);

    PeakList(const PeakList& other)=default;

    PeakList(PeakList&& other)=default;

    ~PeakList()=default;

    PeakList& operator=(const PeakList& other)=default;

    PeakList& operator=(PeakList&& other)=default;

    sptrPeak3D operator[](int index);

    const sptrPeak3D operator[](int index) const;

    void clear();

    bool empty() const;

    void reserve(size_t capacity);

    void add(sptrPeak3D peak);

    void push_back(sptrPeak3D peak);

    iterator remove(sptrPeak3D peak);

    size_t size() const;

    iterator begin();

    const_iterator begin() const;

    iterator end();

    const_iterator end() const;

    const_iterator cbegin() const;

    const_iterator cend() const;

private:

    std::vector<sptrPeak3D> _peaks;
};

} // end namespace nsx
