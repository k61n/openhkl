//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/SpoilerCheck.h
//! @brief     Defines class SubframeFilterPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_UTILITY_SPOILERCHECK_H
#define NSX_GUI_UTILITY_SPOILERCHECK_H

#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>

class SpoilerCheck : public QGroupBox {
    Q_OBJECT

 public:
    QScrollArea contentArea;
    explicit SpoilerCheck(
        const QString& title = "", const int animationDuration = 100, QWidget* parent = 0);
    void setContentLayout(QLayout& contentLayout, bool toggled = false);
    bool checked() const;

 public slots:
    void toggler(const bool check);
    void checker(const int state);

 private:
    QGridLayout mainLayout;
    QToolButton toggleButton;
    QFrame headerLine;
    QParallelAnimationGroup toggleAnimation;
    int animationDuration {300};
    QCheckBox select;
};

#endif // NSX_GUI_UTILITY_SPOILERCHECK_H
