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

//! Scroll area used as the pane where the properties are shown/edited.
//! This QScrollBar derived class prevents the horizontal scrollbar from appearing ever, and takes
//! care of proper resizing when the contents sizes change, the main window size changes or the
//! vertical scrollbars appears/disappears. It accepts only a layout instead of a widget (in
//! opposite to QScrollBar).
class PropertyScrollArea : public QScrollArea {

 public:
    PropertyScrollArea(QWidget* parent);

    //! Set the layout which shall be contained in this instance.
    void setContentLayout(QLayout* layout);

 protected:
    bool eventFilter(QObject* obj, QEvent* ev);

 private:
    using QScrollArea::setWidget; // hiding from access

    void updateFixedWidth();
};


#endif // NSX_GUI_UTILITY_PROPERTYSCROLLAREA_H
