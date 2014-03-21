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

#ifndef NSXTOOL_IDETECTORMAPPING_H_
#define NSXTOOL_IDETECTORMAPPING_H_

namespace SX
{

namespace Instrument
{

typedef unsigned int uint;

class IDetectorMapping
{
public:

	template <typename ...args>
	virtual IDetectorMapping* construct(args...)=0;

	virtual void operator()(const uint nrows, const uint ncols, const uint i, const uint j, uint& newi, uint& newj) const=0;

	virtual ~IDetectorMapping()=0;

protected:
	IDetectorMapping();

};

IDetectorMapping::IDetectorMapping()
{
}

IDetectorMapping::~IDetectorMapping()
{
}

class BottomLeftMapping : public IDetectorMapping
{
public:

	template <typename ...args>
	IDetectorMapping* construct(args...);
	void operator()(const uint nrows, const uint ncols, const uint i, const uint j, uint& newi, uint& newj) const;

};

template <typename ...args>
IDetectorMapping* BottomLeftMapping::construct(args...)
{
	return new BottomLeftMapping;
}

void BottomLeftMapping::operator()(const uint nrows, const uint ncols, const uint i, const uint j, uint& newi, uint& newj) const
{
	newi=i;
	newj=j;
}

//class TopLeftMapping : public IDetectorMapping
//{
//public:
//
//	IDetectorMapping* construct();
//	void operator()(const uint nrows, const uint ncols, const uint i, const uint j, uint& newi, uint& newj) const;
//
//};
//
//IDetectorMapping* TopLeftMapping::construct()
//{
//	return new BottomLeftMapping;
//}
//
//void TopLeftMapping::operator()(const uint nrows, const uint ncols, const uint i, const uint j, uint& newi, uint& newj) const
//{
//	newi=nrows-i;
//	newj=j;
//}
//
//class BottomRightMapping : public IDetectorMapping
//{
//public:
//
//	IDetectorMapping* construct();
//	void operator()(const uint nrows, const uint ncols, const uint i, const uint j, uint& newi, uint& newj) const;
//
//};
//
//IDetectorMapping* BottomRightMapping::construct()
//{
//	return new BottomLeftMapping;
//}
//
//void BottomRightMapping::operator()(const uint nrows, const uint ncols, const uint i, const uint j, uint& newi, uint& newj) const
//{
//	newi=i;
//	newj=ncols-j;
//}
//
//class TopRightMapping : public IDetectorMapping
//{
//public:
//
//	IDetectorMapping* construct();
//	void operator()(const uint nrows, const uint ncols, const uint i, const uint j, uint& newi, uint& newj) const;
//
//};
//
//IDetectorMapping* TopRightMapping::construct()
//{
//	return new BottomLeftMapping;
//}
//
//void TopRightMapping::operator()(const uint nrows, const uint ncols, const uint i, const uint j, uint& newi, uint& newj) const
//{
//	newi=nrows-i;
//	newj=ncols-j;
//}

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_IDETECTORMAPPING_H_ */
