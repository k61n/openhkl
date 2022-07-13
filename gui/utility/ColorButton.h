//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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

#ifndef OHKL_GUI_UTILITY_COLORBUTTON_H
#define OHKL_GUI_UTILITY_COLORBUTTON_H

#include <QPushButton>
#include <QtGui>

class ColorButton : public QPushButton {
    Q_OBJECT
 public:
    explicit ColorButton(const QColor& color = Qt::black, QWidget* parent = 0);

    //! The current color
    QColor color();

 signals:
    //! Emitted whenever the color changes (either by dialog or by direct call to setColor)
    void colorChanged(QColor);

 public slots:
    //! Set the color
    void setColor(const QColor& color);

 private:
    //! Open the color chooser Dialog
    void chooseColor();

 protected:
    virtual void paintEvent(QPaintEvent* event) override;

 private:
    QColor _color;
};

#endif // OHKL_GUI_UTILITY_COLORBUTTON_H