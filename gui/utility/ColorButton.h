//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/ColorButton.h
//! @brief     Defines class ColorButton
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_UTILITY_COLORBUTTON_H
#define NSX_GUI_UTILITY_COLORBUTTON_H

#include <QPushButton>
#include <QtGui>

class ColorButton : public QPushButton {
    Q_OBJECT
 public:
    explicit ColorButton(const QColor& color = Qt::black, QWidget* parent = 0);
    QColor getColor();

 signals:
    void colorChanged(QColor);

 public slots:
    void changeColor(const QColor&);
    void chooseColor();

 protected:
    virtual void paintEvent(QPaintEvent* event) override;

 private:
    QColor _currentColor;
};

#endif // NSX_GUI_UTILITY_COLORBUTTON_H