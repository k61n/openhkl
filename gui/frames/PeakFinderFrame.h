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

#include "gui/graphics/DetectorView.h"
#include "gui/views/PeakTableView.h"

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

#include <QDialogButtonBox>
#include <QTableWidget>
#include <QWidget>
#include <QPushButton>

#include <QHBoxLayout>
#include <QVBoxLayout>

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
   //! Set the execution buttons up
   void setExecuteUp();
   //! Set the parameters values up
   void setParametersUp();
   //!convolution parameter map
   std::map<std::string, double> convolutionParameters();

private:
   QHBoxLayout* main_layout;
   QVBoxLayout* left_layout;

   QcrSpinBox* _threshold_spin;
   QcrDoubleSpinBox* _scale_spin;
   QcrSpinBox* _min_size_spin;
   QcrSpinBox* _max_size_spin;
   QcrSpinBox* _max_width_spin;
   QComboBox* _kernel_combo;
   QTableWidget* _kernel_para_table;
   QcrSpinBox* _start_frame_spin;
   QcrSpinBox* _end_frame_spin;

   QComboBox* data;
   QcrSpinBox* frame;
   QcrCheckBox* applyThreshold;
   QcrDoubleSpinBox* peakArea;
   QcrDoubleSpinBox* backgroundLowerLimit;
   QcrDoubleSpinBox* backgroundUpperLimit;

   DetectorView* preview;
   QDialogButtonBox* buttons;
   QGraphicsPixmapItem* pixmap;

   QPushButton* _find_button;
   QPushButton* _integrate_button;
   QPushButton* _save_button;

   QSizePolicy* _size_policy_widgets;
   QSizePolicy* _size_policy_box;
   QSizePolicy* _size_policy_figure;

   void refreshPreview();
   void accept();
   void doActions(QAbstractButton*);
};

#endif // GUI_FRAMES_PEAKFINDERFRAME_H
