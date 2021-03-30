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

#ifndef NSX_GUI_UTILITY_SPOILER_H
#define NSX_GUI_UTILITY_SPOILER_H

#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>

class Spoiler : public QGroupBox {
    Q_OBJECT

 public:
    QScrollArea contentArea;
    explicit Spoiler(
        const QString& title = "", const int animationDuration = 100, QWidget* parent = 0);
    void setContentLayout(QLayout& contentLayout, bool expanded = false);
    void setExpanded(bool expand);

 public slots:
    void toggler(const bool check); // #nsxUI make private; use setExpanded instead

 private:
    QGridLayout _mainLayout;
    QToolButton _toggleButton;
    QFrame _headerLine;
    QParallelAnimationGroup _toggleAnimation;
    int _animationDuration{300};
};

#endif // NSX_GUI_UTILITY_SPOILER_H
