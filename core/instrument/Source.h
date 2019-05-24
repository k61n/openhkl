//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/Source.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_INSTRUMENT_SOURCE_H
#define CORE_INSTRUMENT_SOURCE_H

#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "Component.h"
#include "Monochromator.h"

namespace nsx {

//! Class encapsulating the source (incoming beam)
class Source : public Component {

public:
    //! Static constructor of a monochromatic source from a yaml tree node
    static Source* create(const YAML::Node& node);
    Source();
    Source(const Source& other);

    //! Constructs a default source with a given name
    Source(const std::string& name);

    //! Constructs a source from a property tree node
    Source(const YAML::Node& node);
    Source* clone() const;
    ~Source();

    //! Assignment operator
    Source& operator=(const Source& other);

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

protected:
    std::vector<Monochromator> _monochromators;
    size_t _selectedMonochromator;
};

} // end namespace nsx

#endif // CORE_INSTRUMENT_SOURCE_H
