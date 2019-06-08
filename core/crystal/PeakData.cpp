//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/crystal/PeakData.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "core/crystal/PeakData.h"
#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/detector/DetectorEvent.h"
#include "core/instrument/Diffractometer.h"
#include "core/geometry/DirectVector.h"
#include "core/instrument/InstrumentState.h"
#include "core/crystal/Peak3D.h"
#include "core/rec_space/ReciprocalVector.h"

namespace nsx {

PeakData::PeakData(sptrPeak3D peak) : _peak(peak), _system(peak), _events(), _counts(), _coords() {}

const std::deque<DetectorEvent>& PeakData::events() const
{
    return _events;
}

const std::deque<double>& PeakData::counts() const
{
    return _counts;
}

void PeakData::computeStandard()
{
    if (_peak == nullptr) {
        throw std::runtime_error(
            "PeakData::computeStandard() cannot be called if _peak is nullptr");
    }

    _coords.resize(_events.size());

    for (size_t i = 0; i < _events.size(); ++i) {
        _coords[i] = _system.transform(_events[i]);
    }
}

void PeakData::addEvent(const DetectorEvent& ev, double count)
{
    _events.push_back(ev);
    _counts.push_back(count);
}

void PeakData::reset()
{
    std::deque<DetectorEvent> e;
    std::deque<double> c;
    std::deque<Eigen::Vector3d> crds;

    std::swap(_events, e);
    std::swap(_counts, c);
    std::swap(_coords, crds);
}

} // end namespace nsx
