//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_filter/SubframeFilterPeaks.h
//! @brief     Defines class SubframeFilterPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_SUBFRAME_FILTER_SUBFRAMEFILTERPEAKS_H
#define GUI_SUBFRAME_FILTER_SUBFRAMEFILTERPEAKS_H

#include "core/peak/PeakCollection.h"

#include "gui/graphics/DetectorView.h"
#include "gui/views/PeakTableView.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/views/PeakTableView.h"
#include "gui/utility/ColorButton.h"

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
#include <QGroupBox>

//! Dialog to filter a peak list
class PeakFilterDialog : public QWidget {
public:

   PeakFilterDialog();
   ~PeakFilterDialog();

   void refreshData();

public:
   void accept();
   void slotActionClicked(QAbstractButton* button);
   void slotUnitCellChanged(int index);


private:

   //! Set up the GUI size policies
   void setSizePolicies();
   //! Build the input
   void setInputUp();
   //! Build the state
   void setStateUp();
   //! Build the unit-call
   void setUnitCellUp();
   //! Build the strength
   void setStrengthUp();
   //! Build the range
   void setRangeUp();
   //! Build the merge
   void setMergeUp();

private:

   //! Run the filtering method
   void filterPeaks();

private:

   //! The temporary collection
   PeakCollectionModel* _peak_collection_model = 
      new PeakCollectionModel();
   //! The temporary collection
   PeakCollectionItem* _peak_collection_item;
   //! The model for the found peaks
   nsx::PeakCollection* _peak_collection = 
      new nsx::PeakCollection("temp", nsx::listtype::FOUND);
   //! The available peak lists
   QList<nsx::sptrDataSet> _peak_list;

private:

   QHBoxLayout* _main_layout;
   QVBoxLayout* _left_layout;
   QSplitter* _right_element;

   QCheckBox* _selected;
   QCheckBox* _masked;
   QCheckBox* _predicted;
   QCheckBox* _indexed_peaks;
   QCheckBox* _extinct_spacegroup;
   QCheckBox* _remove_overlaping;
   QCheckBox* _keep_complementary;
   QCheckBox* _unit_cell;

   QDoubleSpinBox* _tolerance;
   QDoubleSpinBox* _strength_min;
   QDoubleSpinBox* _strength_max;
   QDoubleSpinBox* _d_range_min;
   QDoubleSpinBox* _d_range_max;
   QDoubleSpinBox* _significance_level;

   QSpinBox* _min_number_peaks;
   PeaksTableView* _peaks_table;

   QGroupBox* _input_box;
   QGroupBox* _state_box;
   QGroupBox* _unit_cell_box;
   QGroupBox* _strength_box;
   QGroupBox* _d_range_box;
   QGroupBox* _sparse_box;
   QGroupBox* _merge_box;

   QComboBox* _exp_combo;
   QComboBox* _peak_combo;

   QSizePolicy* _size_policy_widgets;
   QSizePolicy* _size_policy_box;
   QSizePolicy* _size_policy_right;
   QSizePolicy* _size_policy_fixed;

};


#endif // GUI_SUBFRAME_FILTER_SUBFRAMEFILTERPEAKS_H
