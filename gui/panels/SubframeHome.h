//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeHome.h
//! @brief     Defines class SubframeHome
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_SUBFRAMEHOME_H
#define GUI_PANELS_SUBFRAMEHOME_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

class SubframeHome : public QcrWidget {
 public:
    SubframeHome();

 private:
    void createAndLoad();
    void createAndImportRaw();
    QcrLineEdit* expName;
};

#endif // GUI_PANELS_SUBFRAMEHOME_H
