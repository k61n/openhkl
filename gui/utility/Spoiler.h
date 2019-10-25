//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/Spoiler.h
//! @brief     Defines class SubframeFilterPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_UTILITIES_SPOILER
#define GUI_UTILITIES_SPOILER

#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QGroupBox>

class Spoiler : public QGroupBox {
    Q_OBJECT

public:
    QScrollArea contentArea;
    explicit Spoiler(const QString & title = "", const int animationDuration = 100, QWidget *parent = 0);
    void setContentLayout(QLayout & contentLayout, bool toggled = false);

public slots:
    void toggler(const bool check);
    
private:
    QGridLayout mainLayout;
    QToolButton toggleButton;
    QFrame headerLine;
    QParallelAnimationGroup toggleAnimation;
    int animationDuration{300};

};

#endif //GUI_UTILITIES_SPOILER