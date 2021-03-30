//  ************************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/PropertyScrollArea.h
//! @brief     Defines class PropertyScrollArea
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ************************************************************************************************

#ifndef NSX_GUI_UTILITY_PROPERTYSCROLLAREA_H
#define NSX_GUI_UTILITY_PROPERTYSCROLLAREA_H

#include <QEvent>
#include <QScrollArea>

// #nsxUI add docu

class PropertyScrollArea : public QScrollArea {

 public:
    PropertyScrollArea(QWidget* parent);

    void setContentLayout(QLayout* layout);

 protected:
    bool eventFilter(QObject* obj, QEvent* ev);

 private:
    using QScrollArea::setWidget; // hiding from access

    void updateFixedWidth();
};


#endif // NSX_GUI_UTILITY_PROPERTYSCROLLAREA_H
