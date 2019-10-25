//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/ShapeLibraryDialog.h
//! @brief     Defines class ShapeLibraryDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_SUBFRAME_PREDICT_SHAPELIBRARYDIALOG_H
#define GUI_SUBFRAME_PREDICT_SHAPELIBRARYDIALOG_H

#include "core/peak/PeakCollection.h"
#include "gui/models/PeakCollectionModel.h"

#include "core/experiment/DataTypes.h"
#include "core/peak/Peak3D.h"
#include "core/shape/Profile3D.h"
#include "core/shape/ShapeLibrary.h"
#include "gui/models/ColorMap.h"

#include <QGraphicsView>
#include <QTableView>
#include <set>

#include <QGroupBox>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QSizePolicy>
#include <QDialog>

//! Dialog for building the shape library
class ShapeLibraryDialog : public QDialog {

public:

   ShapeLibraryDialog(nsx::PeakCollection* peak_collection);

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

   nsx::ShapeLibrary _library;
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

   QPushButton* _build_library;

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

#endif // GUI_DIALOGS_SHAPELIBRARYDIALOG_H
