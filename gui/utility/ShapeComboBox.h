//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/ShapeComboBox.h
//! @brief     Defines class ShapeComboBox
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_UTILITY_SHAPECOMBO_H
#define OHKL_GUI_UTILITY_SHAPECOMBO_H

#include <QComboBox>

namespace ohkl {
class ShapeModel;
}

using ShapesList = std::vector<ohkl::ShapeModel*>;

//! A QComboBox that that is synchronised with all other QComboBoxes of the same type
class ShapeComboBox : public QComboBox {
    Q_OBJECT

 public:
    ShapeComboBox(QWidget* parent = nullptr);

    //! Add a cell via its pointer
    void addShapeModel(ohkl::ShapeModel* shapes);

    //! Add a list of unit cells
    void addShapeModels(const ShapesList& shapes_list);

    //! Clear all elements
    void clearAll();

    //! Return a pointer to the current unit cell
    ohkl::ShapeModel* currentShapes() const;

    //! Refresh the combo box text
    void refresh();

    //! Refresh all combos of this type
    void refreshAll();

 private:
    //! Index-sorted list of pointers to unit cells
    static ShapesList _shape_models;
    //! Current unit cell
    QString _current;
    //! Vector of all instances to refresh all in one call
    static QVector<ShapeComboBox*> _all_combos;
};

#endif // OHKL_GUI_UTILITY_SHAPECOMBO_H
