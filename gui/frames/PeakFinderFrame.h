//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/PeakFinderFrame.h
//! @brief     Defines classes FoundPeaks, PeakFinderFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_FRAMES_PEAKFINDERFRAME_H
#define GUI_FRAMES_PEAKFINDERFRAME_H

#include "core/peak/PeakCollection.h"

#include "gui/graphics/DetectorView.h"
#include "gui/views/PeakTableView.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/views/PeakTableView.h"

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

#include <QDialogButtonBox>
#include <QTableWidget>
#include <QWidget>
#include <QPushButton>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>

#include <QSizePolicy>


//! Frame which shows the settings to find peaks
class PeakFinderFrame : public QcrFrame {
public:
   PeakFinderFrame();
   //! Change the convolution parameters
   void updateConvolutionParameters();
   //! Find peaks
   void find();
   // ! integrate found peaks
   void integrate();

private:
   //! Set up the GUI size policies
   void setSizePolicies();
   //! Set up the blob finding GUI
   void setBlobUp();
   //! Set up the Preview GUI
   void setPreviewUp();
   //! Set up the Preview GUI
   void setIntegrateUp();
   //! Set up the detector figure up
   void setFigureUp();
   //! Set the peak table view up
   void setPeakTableUp();
   //! Set the execution buttons up
   void setExecuteUp();
   //! Set the parameters values up
   void setParametersUp();

   //! Refresh the preview
   void refreshPreview();
   //! Refresh the found peaks list
   void refreshPeakTable();
   //! Accept and save current list
   void accept();

private:
   //! Convolution parameter map
   std::map<std::string, double> convolutionParameters();
   //! The temporary collection
   PeakCollectionModel* _peak_collection_model = 
      new PeakCollectionModel();
   //! The temporary collection
   PeakCollectionItem* _peak_collection_item;
   //! The model for the found peaks
   nsx::PeakCollection* _peak_collection = 
      new nsx::PeakCollection( "temp", nsx::listtype::FOUND);

private:
   QHBoxLayout* _main_layout;

   QVBoxLayout* _left_layout;
   QSplitter* _right_element;

   QcrSpinBox* _threshold_spin;
   QcrDoubleSpinBox* _scale_spin;
   QcrSpinBox* _min_size_spin;
   QcrSpinBox* _max_size_spin;
   QcrSpinBox* _max_width_spin;
   QComboBox* _kernel_combo;
   QTableWidget* _kernel_para_table;
   QcrSpinBox* _start_frame_spin;
   QcrSpinBox* _end_frame_spin;

   QComboBox* _data_combo;
   QcrSpinBox* _frame_spin;
   QcrCheckBox* _live_check;

   QcrDoubleSpinBox* _peak_area;
   QcrDoubleSpinBox* _bkg_lower;
   QcrDoubleSpinBox* _bkg_upper;

   DetectorView* _figure;
   QGraphicsPixmapItem* _pixmap;

   PeaksTableView* _peak_table;

   QPushButton* _find_button;
   QPushButton* _integrate_button;
   QPushButton* _save_button;

   QSizePolicy* _size_policy_widgets;
   QSizePolicy* _size_policy_box;
   QSizePolicy* _size_policy_right;

};

#endif // GUI_FRAMES_PEAKFINDERFRAME_H
