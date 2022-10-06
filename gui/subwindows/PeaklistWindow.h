//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/PeaklistWindow.cpp
//! @brief     Implements class PeaklistWindow
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBWINDOWS_PEAKLIST_WINDOW_H
#define OHKL_GUI_SUBWINDOWS_PEAKLIST_WINDOW_H

#include "core/experiment/Experiment.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/widgets/PeakProperties.h"
#include <QDialog>

class PeaklistWindow : public QDialog {
 public:
    PeaklistWindow(QWidget* parent = nullptr);
    void refreshAll();
    void setPeakCollection(const QString& pc_name);

 private:
    PeakProperties* _peak_properties;
};

#endif // OHKL_GUI_SUBWINDOWS_PEAKLIST_WINDOW_H