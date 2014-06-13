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

	virtual void operator()(const uint px, const uint py, uint& newpx, uint& newpy) const=0;

	virtual ~IDetectorMapping()=0;

protected:
	IDetectorMapping(const uint nrows, const uint ncols);
	uint _nrows;
	uint _ncols;

};

IDetectorMapping::IDetectorMapping(const uint nrows, const uint ncols) : _nrows(nrows), _ncols(ncols)
{
}

void IDetectorMapping::operator()(const uint px, const uint py, uint& newpx, uint& newpy) const
{
}

IDetectorMapping::~IDetectorMapping()
{
}

///////////////////////////////////////////////

class BottomLeftMappingCCW : public IDetectorMapping
{
public:

	static IDetectorMapping* create(const uint nrows, const uint ncols);
	void operator()(const uint px, const uint py, uint& newpx, uint& newpy) const;

protected:
	BottomLeftMappingCCW(const uint nrows, const uint ncols);

};

BottomLeftMappingCCW::BottomLeftMappingCCW(const uint nrows, const uint ncols) : IDetectorMapping(nrows,ncols)
{
}

IDetectorMapping* BottomLeftMappingCCW::create(const uint nrows, const uint ncols)
{
	return new BottomLeftMappingCCW(nrows,ncols);
}

void BottomLeftMappingCCW::operator()(const uint px, const uint py, uint& newpx, uint& newpy) const
{
	newpx=px;
	newpy=py;
}

///////////////////////////////////////////////

class BottomLeftMappingCW : public IDetectorMapping
{
public:

	static IDetectorMapping* create(const uint nrows, const uint ncols);
	void operator()(const uint px, const uint py, uint& newpx, uint& newpy) const;

protected:
	BottomLeftMappingCW(const uint nrows, const uint ncols);

};

BottomLeftMappingCW::BottomLeftMappingCW(const uint nrows, const uint ncols) : IDetectorMapping(nrows,ncols)
{
}

IDetectorMapping* BottomLeftMappingCW::create(const uint nrows, const uint ncols)
{
	return new BottomLeftMappingCW(nrows,ncols);
}

void BottomLeftMappingCW::operator()(const uint px, const uint py, uint& newpx, uint& newpy) const
{
	newpx=py;
	newpy=px;
}

///////////////////////////////////////////////

class TopLeftMappingCCW : public IDetectorMapping
{
public:

	static IDetectorMapping* create(const uint nrows, const uint ncols);
	void operator()(const uint px, const uint py, uint& newpx, uint& newpy) const;

protected:
	TopLeftMappingCCW(const uint nrows, const uint ncols);

};

TopLeftMappingCCW::TopLeftMappingCCW(const uint nrows, const uint ncols) : IDetectorMapping(nrows,ncols)
{
}

IDetectorMapping* TopLeftMappingCCW::create(const uint nrows, const uint ncols)
{
	return new TopLeftMappingCCW(nrows,ncols);
}

void TopLeftMappingCCW::operator()(const uint px, const uint py, uint& newpx, uint& newpy) const
{
	newpx=py;
	newpy=_nrows-px;
}

///////////////////////////////////////////////

class TopLeftMappingCW : public IDetectorMapping
{
public:

	static IDetectorMapping* create(const uint nrows, const uint ncols);
	void operator()(const uint px, const uint py, uint& newpx, uint& newpy) const;

protected:
	TopLeftMappingCW(const uint nrows, const uint ncols);

};

TopLeftMappingCW::TopLeftMappingCW(const uint nrows, const uint ncols) : IDetectorMapping(nrows,ncols)
{
}

IDetectorMapping* TopLeftMappingCW::create(const uint nrows, const uint ncols)
{
	return new TopLeftMappingCW(nrows,ncols);
}

void TopLeftMappingCW::operator()(const uint px, const uint py, uint& newpx, uint& newpy) const
{
	newpx=px;
	newpy=_nrows-py;
}

///////////////////////////////////////////////

class BottomRightMappingCCW : public IDetectorMapping
{
public:

	static IDetectorMapping* create(const uint nrows, const uint ncols);
	void operator()(const uint px, const uint py, uint& newpx, uint& newpy) const;

protected:
	BottomRightMappingCCW(const uint nrows, const uint ncols);

};

BottomRightMappingCCW::BottomRightMappingCCW(const uint nrows, const uint ncols) : IDetectorMapping(nrows,ncols)
{
}

IDetectorMapping* BottomRightMappingCCW::create(const uint nrows, const uint ncols)
{
	return new BottomRightMappingCCW(nrows,ncols);
}

void BottomRightMappingCCW::operator()(const uint px, const uint py, uint& newpx, uint& newpy) const
{
	newpx=_ncols-py;
	newpy=px;
}

///////////////////////////////////////////////

class BottomRightMappingCW : public IDetectorMapping
{
public:

	static IDetectorMapping* create(const uint nrows, const uint ncols);
	void operator()(const uint px, const uint py, uint& newpx, uint& newpy) const;

protected:
	BottomRightMappingCW(const uint nrows, const uint ncols);

};

BottomRightMappingCW::BottomRightMappingCW(const uint nrows, const uint ncols) : IDetectorMapping(nrows,ncols)
{
}

IDetectorMapping* BottomRightMappingCW::create(const uint nrows, const uint ncols)
{
	return new BottomRightMappingCW(nrows,ncols);
}

void BottomRightMappingCW::operator()(const uint px, const uint py, uint& newpx, uint& newpy) const
{
	newpx=_ncols-px;
	newpy=py;
}

///////////////////////////////////////////////

class TopRightMappingCCW : public IDetectorMapping
{
public:

	static IDetectorMapping* create(const uint nrows, const uint ncols);
	void operator()(const uint px, const uint py, uint& newpx, uint& newpy) const;

protected:
	TopRightMappingCCW(const uint nrows, const uint ncols);

};

TopRightMappingCCW::TopRightMappingCCW(const uint nrows, const uint ncols) : IDetectorMapping(nrows,ncols)
{
}

IDetectorMapping* TopRightMappingCCW::create(const uint nrows, const uint ncols)
{
	return new TopRightMappingCCW(nrows,ncols);
}

void TopRightMappingCCW::operator()(const uint px, const uint py, uint& newpx, uint& newpy) const
{
	newpx=_ncols-px;
	newpy=_nrows-py;
}

///////////////////////////////////////////////

class TopRightMappingCW : public IDetectorMapping
{
public:

	static IDetectorMapping* create(const uint nrows, const uint ncols);
	void operator()(const uint px, const uint py, uint& newpx, uint& newpy) const;

protected:
	TopRightMappingCW(const uint nrows, const uint ncols);

};

TopRightMappingCW::TopRightMappingCW(const uint nrows, const uint ncols) : IDetectorMapping(nrows,ncols)
{
}

IDetectorMapping* TopRightMappingCW::create(const uint nrows, const uint ncols)
{
	return new TopRightMappingCW(nrows,ncols);
}

void TopRightMappingCW::operator()(const uint px, const uint py, uint& newpx, uint& newpy) const
{
	newpx=_ncols-py;
	newpy=_nrows-px;
}

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_IDETECTORMAPPING_H_ */
