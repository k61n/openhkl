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

//! A group box which can be collapsed/expanded.
class Spoiler : public QGroupBox {
    Q_OBJECT

 public:
    //! If the group box shall be checkable, the headline contains a checkbox. If this checkbox is
    //! unchecked, all the contents will be disabled.
    explicit Spoiler(const QString& title, bool isCheckable = false);
    void setContentLayout(QLayout& contentLayout, bool expanded = false); // #nsxUI ptr instead ref
    QLayout* contentLayout();

    //! Set the expanded state.
    void setExpanded(bool expand);

    //! True if expanded
    bool isExpanded() const;

    //! If the spoiler is checkable (see constructor), then this returns the checked-state. Returns
    //! false if not checkable at all.
    bool isChecked() const;

    //! If the spoiler is checkable (see constructor), set the checked state.
    //! No effect if not checkable at all.
    void setChecked(bool checked);

    //! Get a pointer to the QCheckBox
    QCheckBox* checkBox() { return _select; };

 private slots:
    void checker(const int state);
    void showEvent(QShowEvent* event);
    void onAnimationFinished();

 private:
    void toggler(const bool check);

 private:
    QGridLayout* _mainLayout = nullptr;
    QToolButton* _toggleButton = nullptr;
    QCheckBox* _select = nullptr;
    QWidget* _contentArea = nullptr;

    QParallelAnimationGroup _toggleAnimation;
    int _animationDuration{300};
};

#endif // NSX_GUI_UTILITY_SPOILER_H
