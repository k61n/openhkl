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

	virtual void operator()(const uint i, const uint j, uint& newi, uint& newj) const=0;

	virtual ~IDetectorMapping()=0;

protected:
	IDetectorMapping(const uint nrows, const uint ncols);
	uint _nrows;
	uint _ncols;

};

IDetectorMapping::IDetectorMapping(const uint nrows, const uint ncols) : _nrows(nrows), _ncols(ncols)
{
}

void IDetectorMapping::operator()(const uint i, const uint j, uint& newi, uint& newj) const
{
}

IDetectorMapping::~IDetectorMapping()
{
}

///////////////////////////////////////////////

class BottomLeftMapping : public IDetectorMapping
{
public:

	static IDetectorMapping* construct(const uint nrows, const uint ncols);
	void operator()(const uint i, const uint j, uint& newi, uint& newj) const;

protected:
	BottomLeftMapping(const uint nrows, const uint ncols);

};

BottomLeftMapping::BottomLeftMapping(const uint nrows, const uint ncols) : IDetectorMapping(nrows,ncols)
{
}

IDetectorMapping* BottomLeftMapping::construct(const uint nrows, const uint ncols)
{
	return new BottomLeftMapping(nrows,ncols);
}

void BottomLeftMapping::operator()(const uint i, const uint j, uint& newi, uint& newj) const
{
	newi=i;
	newj=j;
}

///////////////////////////////////////////////

class TopLeftMapping : public IDetectorMapping
{
public:

	static IDetectorMapping* construct(const uint nrows, const uint ncols);
	void operator()(const uint i, const uint j, uint& newi, uint& newj) const;

protected:
	TopLeftMapping(const uint nrows, const uint ncols);

};

TopLeftMapping::TopLeftMapping(const uint nrows, const uint ncols) : IDetectorMapping(nrows,ncols)
{
}

IDetectorMapping* TopLeftMapping::construct(const uint nrows, const uint ncols)
{
	return new TopLeftMapping(nrows,ncols);
}

void TopLeftMapping::operator()(const uint i, const uint j, uint& newi, uint& newj) const
{
	newi=_nrows-i;
	newj=j;
}

///////////////////////////////////////////////

class BottomRightMapping : public IDetectorMapping
{
public:

	static IDetectorMapping* construct(const uint nrows, const uint ncols);
	void operator()(const uint i, const uint j, uint& newi, uint& newj) const;

protected:
	BottomRightMapping(const uint nrows, const uint ncols);

};

BottomRightMapping::BottomRightMapping(const uint nrows, const uint ncols) : IDetectorMapping(nrows,ncols)
{
}

IDetectorMapping* BottomRightMapping::construct(const uint nrows, const uint ncols)
{
	return new BottomRightMapping(nrows,ncols);
}

void BottomRightMapping::operator()(const uint i, const uint j, uint& newi, uint& newj) const
{
	newi=i;
	newj=_ncols-j;
}

///////////////////////////////////////////////

class TopRightMapping : public IDetectorMapping
{
public:

	static IDetectorMapping* construct(const uint nrows, const uint ncols);
	void operator()(const uint i, const uint j, uint& newi, uint& newj) const;

protected:
	TopRightMapping(const uint nrows, const uint ncols);

};

TopRightMapping::TopRightMapping(const uint nrows, const uint ncols) : IDetectorMapping(nrows,ncols)
{
}

IDetectorMapping* TopRightMapping::construct(const uint nrows, const uint ncols)
{
	return new TopRightMapping(nrows,ncols);
}

void TopRightMapping::operator()(const uint i, const uint j, uint& newi, uint& newj) const
{
	newi=_nrows-i;
	newj=_ncols-j;
}

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_IDETECTORMAPPING_H_ */
