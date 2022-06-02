//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/ShapeComboBox.h
//! @brief     Defines class ShapeComboBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_UTILITY_SHAPECOMBO_H
#define NSX_GUI_UTILITY_SHAPECOMBO_H

#include "core/shape/ShapeModel.h"

#include <QComboBox>

namespace nsx
{
class ShapeModel;
}

using ShapesList = std::vector<nsx::ShapeModel*>;

//! A QComboBox that that is synchronised with all other QComboBoxes of the same type
class ShapeComboBox : public QComboBox {
    Q_OBJECT

 public:
    ShapeComboBox(QWidget* parent = nullptr);

    //! Add a cell via its pointer
    void addShapeModel(nsx::ShapeModel* shapes);

    //! Add a list of unit cells
    void addShapeModels(const ShapesList& shapes_list);

    //! Clear all elements
    void clearAll();

    //! Return a pointer to the current unit cell
    nsx::ShapeModel* currentShapes() const;

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

#endif // NSX_GUI_UTILITY_SHAPECOMBO_H
