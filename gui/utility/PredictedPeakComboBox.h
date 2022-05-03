//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/PredictedPeakComboBox.h
//! @brief     Defines class PredictedPeakComboBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_UTILITY_PREDICTEDPEAKCOMBO_H
#define NSX_GUI_UTILITY_PREDICTEDPEAKCOMBO_H

#include "gui/utility/PeakComboBox.h"

#include <QComboBox>

namespace nsx
{
class PeakCollection;
}

//! A QComboBox that that is synchronised with all other QComboBoxes of the same type
class PredictedPeakComboBox : public PeakComboBox {
    Q_OBJECT

 public:
    PredictedPeakComboBox(QWidget* parent = nullptr);

    //! Add a PeakCollection via its pointer
    void addPeakCollection(nsx::PeakCollection* peaks);

 private:
    static PeakList _predicted_peaks;
};

#endif // NSX_GUI_UTILITY_PREDICTEDPEAKCOMBO_H
