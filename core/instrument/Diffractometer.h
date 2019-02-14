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

#pragma once

#include <memory>
#include <string>

#include <yaml-cpp/yaml.h>

#include "Detector.h"
#include "Sample.h"
#include "Source.h"

namespace nsx {

//! Class describing a general diffractometer, consisting of a sample, source,
//! and detector.
class Diffractometer {

public:
  static Diffractometer *create(const std::string &name);

public:
  Diffractometer(const YAML::Node &node);

  //! Constructs a diffractometer from another one
  Diffractometer(const Diffractometer &other);

  Diffractometer *clone() const;

  //! Destructor
  virtual ~Diffractometer();

  //! Assignment operator
  Diffractometer &operator=(const Diffractometer &other);

  //! Return the name of this diffractometer
  const std::string &name() const;

  //! Set the name of the diffractometer
  void setName(const std::string &name);

  //! Return a pointer to the detector of this diffractometer
  Detector *detector();

  //! Returns const pointer to the detector of this diffractometer
  const Detector *detector() const;

#ifndef SWIG
  //! Set the detector of this diffractometer
  void setDetector(std::unique_ptr<Detector> detector);
#endif

  //! Return the non-const reference to the sample of this diffractometer
  Sample &sample();

  //! Return the const reference to the sample of this diffractometer
  const Sample &sample() const;

  //! Set the sample of this diffractometer
  void setSample(const Sample &sample);

  //! Return the non-const reference to the source of this diffractometer
  Source &source();

  //! Return the const reference to the source of this diffractometer
  const Source &source() const;

  //! Set the source of this diffractometer
  void setSource(const Source &source);

protected:
  //! Default constructor
  Diffractometer();

  //! Constructs a diffractometer with a given name
  Diffractometer(const std::string &name);

  //! Name of the diffractometer
  std::string _name;

  //! Pointer to detector
  std::unique_ptr<Detector> _detector;

  //! The sample
  Sample _sample;

  //! The neutron incoming beam
  Source _source;
};

} // end namespace nsx
