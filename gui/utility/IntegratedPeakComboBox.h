//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/IntegratedPeakComboBox.h
//! @brief     Defines class IntegratedPeakComboBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_UTILITY_INTEGRATEDPEAKCOMBO_H
#define NSX_GUI_UTILITY_INTEGRATEDPEAKCOMBO_H

#include "gui/utility/PeakComboBox.h"

#include <QComboBox>

namespace nsx
{
class PeakCollection;
}

//! A QComboBox that that is synchronised with all other QComboBoxes of the same type
class IntegratedPeakComboBox : public PeakComboBox {
    Q_OBJECT

 public:
    IntegratedPeakComboBox(QWidget* parent = nullptr);

    //! Add a PeakCollection via its pointer
    void addPeakCollection(nsx::PeakCollection* peaks);

    //! Add a list of peak collection
    void addPeakCollections(const PeakList& peaks);

 private:

    static PeakList _integrated_peaks;
};

#endif // NSX_GUI_UTILITY_INTEGRATEDPEAKCOMBO_H
