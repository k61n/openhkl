//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/GridFiller.h
//! @brief     Defines class GridFiller
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_UTILITY_GRIDFILLER_H
#define NSX_GUI_UTILITY_GRIDFILLER_H

#include <QString>


class Spoiler;
class QGridLayout;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QWidget;
class QCheckBox;
class QPushButton;

//! Helps to fill a grid layout according to the standards used all over NSX (e.g. labels are right
//! aligned).
class GridFiller {

 public:
    GridFiller(QGridLayout* gridLayout);
    GridFiller(Spoiler* spoiler, bool expanded = false);
    ~GridFiller();

    //! Add a row with a label and a combo box. If labelText is empty, then no label is created, and
    //! the tooltip is set on the created combo box directly.
    QComboBox* addCombo(const QString& labelText = QString(), const QString& tooltip = QString());

    //! Add a row with a check box. The check box is placed in the given column, with span to the
    //! right edge.
    QCheckBox* addCheckBox(const QString& title, int col = 0);

    //! Add a row with a check box. The check box is placed in the given column, with span to the
    //! right edge.
    QCheckBox* addCheckBox(const QString& title, const QString& tooltip, int col = 0);

    //! Add a row with a label, a tooltip for the label and a spin box.
    QSpinBox* addSpinBox(const QString& labelText, const QString& labelTooltip = QString());

    //! Add a row with a label, a tooltip for the label and a double spin box.
    QDoubleSpinBox* addDoubleSpinBox(
        const QString& labelText, const QString& labelTooltip = QString());

    //! Add a row with a label, a tooltip for the label and two spin boxes.
    std::tuple<QSpinBox*, QSpinBox*> addSpinBoxPair(
        const QString& labelText, const QString& labelTooltip = QString());

    //! Add a row with a label, a tooltip for the label and two double spin boxes.
    std::tuple<QDoubleSpinBox*, QDoubleSpinBox*> addDoubleSpinBoxPair(
        const QString& labelText, const QString& labelTooltip = QString());

    //! Add a row containing a pushbutton which covers the whole row.
    QPushButton* addButton(const QString& text, const QString& tooltip = QString());

    //! Add a row with the given widget. The widget is inserted in the given column with the given
    //! column span (-1 => to the right edge).
    //! The internal row counter will be increased.
    void addWidget(QWidget* w, int col = 0, int colspan = -1);

 private:
    //! Add a label in the current row. The row will not be increased.
    void addLabel(const QString& labelText, const QString& labelTooltip = QString());

 private:
    QGridLayout* _mainLayout; //!< The layout to fill
    Spoiler* _spoiler;
    int _nextRow; //!< The next row to add
};

#endif // NSX_GUI_UTILITY_GRIDFILLER_H
