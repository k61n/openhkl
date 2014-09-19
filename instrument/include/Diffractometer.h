/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Eric Pellegrini
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
#ifndef NSXTOOL_DIFFRACTOMETER_H_
#define NSXTOOL_DIFFRACTOMETER_H_
#include <string>

namespace SX
{

namespace Instrument
{

class Detector;
class Sample;


class Diffractometer
{
public:
	Diffractometer(const std::string& name);
	~Diffractometer();
	void setDetector(Detector*);
	Detector* getDetector();
	void setSample(Sample*);
	Sample* getSample();

private:
	std::string _name;
	Detector* _detector;
	Sample* _sample;
};

} // Namespace Instrument
} /* Namespace SX */

#endif /* NSXTOOL_DIFFRACTOMETER_H_ */
