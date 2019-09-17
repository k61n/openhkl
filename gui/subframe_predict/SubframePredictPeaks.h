//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_find/SubframeFindPeaks.h
//! @brief     Defines classes FoundPeaks, PeakFinderFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_SUBFRAME_PREDICT_SUBFRAMEPREDICTPEAKS_H
#define GUI_SUBFRAME_PREDICT_SUBFRAMEPREDICTPEAKS_H

#include "core/peak/PeakCollection.h"

#include "gui/graphics/DetectorView.h"
#include "gui/views/PeakTableView.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/views/PeakTableView.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/Spoiler.h"

#include <QTableWidget>
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

//! Frame which shows the settings to find peaks
class SubframePredictPeaks : public QWidget{
public:

   SubframePredictPeaks();
   //! run the auto indexing
   void runPrediction();
   //! Refresh all the panels
   void refreshAll();

private:

   //! Set up the GUI size policies
   void setSizePolicies();
   //! Build the input
   void setInputUp();
   //! Set the parameters values up
   void setParametersUp();
   //! Build the buttons
   void setProceedUp();
   //! Set the peak table view up
   void setPeakTableUp();
   //! Set up the detector figure up
   void setFigureUp();
   //! Set the parameters values up
   void setPreviewUp();

private:

   //! Set the experiments
   void setExperiments();
   //! Update the peak list
   void updatePeakList();
   //! Update the peak list
   void updateUnitCellList();
   //! Update the peak list
   void updateDatasetList();
   //! Update the peak list
   void updateDatasetParameters(int idx);
   //! Get the parameters of the indexer
   void grabPredictorParameters();
   //! Get the parameters of the indexer
   void setPredictorParameters() const;
   //! Refresh the found peaks list
   void refreshPeakTable();
   //! Refresh the found peaks visual properties
   void refreshPeakVisual();
   //! Change the peak selected in the table
   void changeSelected(PeakItemGraphic* peak_graphic);
   //! Open the shape builder dialog
   void openShapeBuilder();
   //! Accept and save current list
   void accept();


private:

   //! The model for the found peaks
   nsx::PeakCollection _peak_collection;
   //! The temporary collection
   PeakCollectionItem _peak_collection_item;
   //! The temporary collection
   PeakCollectionModel _peak_collection_model;
   //! The loaded data list
   QList<nsx::sptrDataSet> _data_list;
   //! The loaded peak list
   QStringList _peak_list;
   //! The loaded peak list
   QStringList _unit_cell_list;

private:
   QHBoxLayout* _main_layout;

   QVBoxLayout* _left_layout;
   QSplitter* _right_element;

   Spoiler* _input_box;
   Spoiler* _para_box;
   Spoiler* _preview_box;

   QComboBox* _unit_cells;
   QComboBox* _interpolation;
   QDoubleSpinBox* _d_min;
   QDoubleSpinBox* _d_max;
   QDoubleSpinBox* _radius;
   QDoubleSpinBox* _n_frames;
   QSpinBox* _min_neighbors;

   QComboBox* _exp_combo;
   QComboBox* _peak_combo;
   QComboBox* _data_combo;

   QCheckBox* _draw_active;
   QCheckBox* _draw_inactive;
   QSpinBox* _width_active;
   QSpinBox* _width_inactive;
   ColorButton* _color_active;
   ColorButton* _color_inactive;
   QCheckBox* _live_check;

   DetectorView* _figure_view;
   QGraphicsPixmapItem* _pixmap;
   QSpinBox* _figure_spin;
   QScrollBar* _figure_scroll;

   PeaksTableView* _peak_table;

   QPushButton* _build_shape_lib;
   QPushButton* _run_prediction;
   QPushButton* _save_button;

   QSizePolicy* _size_policy_widgets;
   QSizePolicy* _size_policy_box;
   QSizePolicy* _size_policy_right;
   QSizePolicy* _size_policy_fixed;
};

#endif // GUI_SUBFRAME_PREDICT_SUBFRAMEPREDICTPEAKS_H
