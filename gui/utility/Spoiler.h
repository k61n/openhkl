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

#include <QGroupBox>
#include <QParallelAnimationGroup>

class QGridLayout;
class QToolButton;
class QCheckBox;

class Spoiler : public QGroupBox {
    Q_OBJECT

 public:
    explicit Spoiler(const QString& title, bool isCheckable = false);
    void setContentLayout(QLayout& contentLayout, bool expanded = false); // #nsxUI ptr instead ref
    QLayout* contentLayout();

    void setExpanded(bool expand);
    bool isExpanded() const;

    bool isChecked() const;
    void setChecked(bool checked);

 public:
    void toggler(const bool check); // #nsxUI make private; use setExpanded instead

 private slots:
    void checker(const int state);
    void showEvent(QShowEvent* event);
    void onAnimationFinished();

 private:
    QGridLayout* _mainLayout = nullptr;
    QToolButton* _toggleButton = nullptr;
    QCheckBox* _select = nullptr;
    QWidget* _contentArea = nullptr;

    QParallelAnimationGroup _toggleAnimation;
    int _animationDuration{300};
};

#endif // NSX_GUI_UTILITY_SPOILER_H
