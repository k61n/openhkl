//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_predict/ShapeCollectionDialog.h
//! @brief     Defines class ShapeCollectionDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_PREDICT_SHAPECOLLECTIONDIALOG_H
#define NSX_GUI_SUBFRAME_PREDICT_SHAPECOLLECTIONDIALOG_H

#include "core/shape/PeakCollection.h"
#include "gui/models/PeakCollectionModel.h"

#include "core/data/DataTypes.h"
#include "core/peak/Peak3D.h"
#include "core/shape/Profile3D.h"
#include "core/shape/ShapeCollection.h"
#include "gui/models/ColorMap.h"

#include <QGraphicsView>
#include <QTableView>
#include <set>

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

//! Dialog for building the shape collection
class ShapeCollectionDialog : public QDialog {
 public:
    ShapeCollectionDialog(nsx::PeakCollection* peak_collection);

 private:
    void calculate();
    void build();
    void drawFrame(int value);
    void selectTargetPeak(int row);
    void accept();
    const nsx::Profile3D& profile() { return _profile; }

 private:
    //! Set up the GUI size policies
    void setSizePolicies();
    //! Build the parameters
    void setParametersUp();
    //! Build the preview
    void setPreviewUp();
    //! Once all the gui elements are set up fill them
    void setUpParametrization(nsx::PeakCollection* peak_collection);

 private:
    //! The temporary collection
    PeakCollectionModel _peak_collection_model;
    //! The temporary collection
    PeakCollectionItem _peak_collection_item;

    nsx::ShapeCollection _collection;
    nsx::sptrUnitCell _unitCell;
    std::vector<nsx::Peak3D*> _peaks;
    std::set<nsx::sptrDataSet> _data;
    nsx::Profile3D _profile;
    nsx::PeakCollection* _collection_ptr;
    double _maximum;
    ColorMap _cmap;

 private:
    QWidget* _parameter_widget;
    QWidget* _preview_widget;

    QSizePolicy* _size_policy_widgets;

    QSpinBox* _nx;
    QSpinBox* _ny;
    QSpinBox* _nz;

    QGroupBox* _kabsch;

    QDoubleSpinBox* _sigma_D;
    QDoubleSpinBox* _sigma_M;

    QDoubleSpinBox* _min_I_sigma;
    QDoubleSpinBox* _min_d;
    QDoubleSpinBox* _max_d;
    QDoubleSpinBox* _peak_scale;
    QDoubleSpinBox* _background_begin;
    QDoubleSpinBox* _background_end;

    QPushButton* _build_collection;

    QTableView* _table;
    QDoubleSpinBox* _x;
    QDoubleSpinBox* _y;
    QDoubleSpinBox* _frame;
    QDoubleSpinBox* _radius;
    QDoubleSpinBox* _n_frames;

    QPushButton* _calculate_mean_profile;

    QGraphicsView* _graphics;
    QSlider* _draw_frame;
};

#endif // NSX_GUI_SUBFRAME_PREDICT_SHAPECOLLECTIONDIALOG_H
