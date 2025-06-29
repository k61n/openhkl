//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/instrument/Source.h
//! @brief     Defines class Source
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_INSTRUMENT_SOURCE_H
#define OHKL_CORE_INSTRUMENT_SOURCE_H

#include "core/gonio/Component.h"
#include "core/instrument/Monochromator.h"

namespace ohkl {

//! A set of `Monochromator`s, of which one is selected.

class Source : public Component {
 public:
    Source() = delete;

    virtual ~Source() = default;

    //! Constructs a source from a property tree node
    Source(const YAML::Node& node);

    Source* clone() const;

    //! Returns the monochromators registered for this Source
    const std::vector<Monochromator>& monochromators() const;

    //! Returns the number of monochromators associated with this source
    int nMonochromators() const;
    //! Select a monochromator for this source

    void setSelectedMonochromator(size_t i);

    //! Returns a non-const reference to the selected monochromator
    Monochromator& selectedMonochromator();

    //! Returns a non-const reference to the selected monochromator
    const Monochromator& selectedMonochromator() const;

    //! Add a new monochromator to this source
    void addMonochromator(Monochromator mono);

    // Source has no goniometer
    const Gonio& gonio() const = delete;
    Gonio& gonio() = delete;

 protected:
    std::vector<Monochromator> _monochromators;
    size_t _selectedMonochromator;
};

} // namespace ohkl

#endif // OHKL_CORE_INSTRUMENT_SOURCE_H
