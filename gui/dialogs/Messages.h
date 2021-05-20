//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/Messages.h
//! @brief     Defines class AboutDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef GUI_DIALOGS_MESSAGES_H
#define GUI_DIALOGS_MESSAGES_H

#define APPLICATION_NAME "NSXTool"
#define APPLICATION_CLAIM "Peak integration for neutron single crystal diffraction"

#include <QDialog>

//! Modal dialog printing general software information
class AboutBox : public QDialog {
 public:
    AboutBox();
};

#endif // GUI_DIALOGS_MESSAGES_H
