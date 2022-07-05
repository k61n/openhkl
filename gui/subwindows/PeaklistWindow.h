//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/PeaklistWindow.cpp
//! @brief     Implements class PeaklistWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBWINDOWS_PEAKLIST_WINDOW_H
#define NSX_GUI_SUBWINDOWS_PEAKLIST_WINDOW_H

#include <QDialog>
#include "gui/widgets/PeakProperties.h"
#include "gui/models/Session.h"
#include "gui/models/Project.h"
#include "core/experiment/Experiment.h"

class PeaklistWindow : public QDialog{
    public:
        PeaklistWindow(QWidget* parent = nullptr);
        void refreshAll();
    private:
        PeakProperties* _peak_properties;
        QComboBox* _data_selector;
};

#endif //NSX_GUI_SUBWINDOWS_PEAKLIST_WINDOW_H