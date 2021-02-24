//  ***********************************************************************************************
//
//  XSECTIONTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/chemistry/Material.h
//! @brief     Defines class Material
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef XSECTION_LIB_MATERIAL_H
#define XSECTION_LIB_MATERIAL_H

#include "IsotopeDatabaseManager.h"

namespace xsection {

//! Class to represent the sample material type.
class Material {

public:
    Material() = delete;

    //! Constructs a Material with a given name
    Material(const std::string& formula);

    Material(const Material& other) = default;
    ~Material() = default;
    Material& operator=(const Material& other) = default;

    Material* clone() const;

    //! Returns the name of this Material
    const std::string& formula() const;

    const isotopeContents& isotopes() const;

    //! Returns the mass density of this Material
    double massDensity() const;
    //! Sets the mass density of this Material
    void setMassDensity(double massDensity);

    //! Returns the molar mass of this Material
    double molarMass() const;

    //! Returns the mass fractions of this Material
    isotopeContents massFractions() const;

    //! Returns the number of atoms per volume units per element (1/m3)
    isotopeContents atomicNumberDensity() const;

    //! Returns the scattering attenuation factor of this Material
    double muIncoherent() const;
    //! Returns the absorption attenuation factor of this Material at a given
    //! wavelength
    double muAbsorption(double lambda = 1.798e-10) const;

    //! Print informations about this Material to an output stream
    void print(std::ostream& os) const;

private:
    std::string _formula;

    isotopeContents _isotopes;

    double _massDensity;
};

#ifndef SWIG
std::ostream& operator<<(std::ostream& os, const Material& material);
#endif

} // namespace xsection

#endif // XSECTION_LIB_MATERIAL_H
