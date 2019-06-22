//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/MessageBoxes.h
//! @brief     Defines classes AboutBox, HelpBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_DIALOGS_MESSAGEBOXES_H
#define GUI_DIALOGS_MESSAGEBOXES_H

#include <QMessageBox>

class AboutBox : public QMessageBox {
 public:
    AboutBox();
};

class HelpBox : public QDialog {
 public:
    HelpBox();
};

#endif // GUI_DIALOGS_MESSAGEBOXES_H
