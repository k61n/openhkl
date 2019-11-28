//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/RefinerFrame.h
//! @brief     Defines classes RefinerFrame, RefinerFitWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_DIALOG_REFINERDIALOG_H
#define GUI_DIALOG_REFINERDIALOG_H

#include "core/algo/Refiner.h"
#include "gui/views/PeakTableView.h"

#include "gui/graphics/SXPlot.h"
#include "gui/utility/QScienceSpinBox.h"

#include <QDialogButtonBox>
#include <QListWidget>
#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>
#include <QSizePolicy>
#include <QDialog>
#include <QComboBox>
#include <QList>

//! Frame which shows the settings for the refiner
class RefinerDialog : public QDialog {

public:

    RefinerDialog(nsx::UnitCell* unit_cell);

private:

   //! Set up the general layout
   void _layout();
   //! Set the size policies
   void _setSizePolicies();
   //! The selected data was changed
   void _selectedDataChanged();

   //! Set the initial values for a frame
   void _setInitialValues(int frame);
   //! Set the initial values for a frame
   void _setRefinedValues(int frame);

   //! Fetch all items 
   void _fetchAllInitialValues();
   //! Fetch all items 
   void _fetchAllRefinedValues();

   //! Set up the input
   void _setInputUp();
   //! Set up the input
   void _setInformationUp();
   //! Set up the input
   void _setGraphUp();

   //! Set up the sample tab up
   void _setSampleUp();
   //! Set up the sample tab up
   void _setDetectorUp();
   //! Set up the sample tab up
   void _setUnitCellUp();
   //! Set up the sample tab up
   void _setInstrumentUp();

   //! Set up the input
   void _setUnitCellDrop();
   //! Set up the input
   void _setPeakList();
   //! Set up the input
   void _setDataList();
   //! Set up the input
   void _setDataDrop();

   //! Accept the found solution
   void accept();
   //! Refine the parameters
   void refine();

private:

   //! 
   void _plot();
   //! 
   void _resetPlot();
   //!
   void _plotPositions();
   //!
   void _plotOrientations();

private: 
   
   nsx::UnitCell* _unit_cell;
   QList<nsx::sptrDataSet> _data_list;

   std::map<nsx::sptrDataSet,std::vector<Eigen::Matrix3d>> _sample_orientations;
   std::map<nsx::sptrDataSet,std::vector<Eigen::Matrix3d>> _detector_orientations;
   std::map<nsx::sptrDataSet,std::vector<Eigen::Vector3d>> _sample_positions;
   std::map<nsx::sptrDataSet,std::vector<Eigen::Vector3d>> _detector_positions;
   std::map<nsx::sptrDataSet,std::vector<Eigen::Vector3d>> _nis;
   std::map<nsx::sptrDataSet,std::vector<double>> _wavelengths;
   std::map<nsx::sptrDataSet,std::vector<double>> _wavelength_offsets;

   std::map<nsx::sptrDataSet,Eigen::Vector3d> _uc_dims;
   std::map<nsx::sptrDataSet,Eigen::Vector3d> _uc_angles;

   std::map<nsx::sptrDataSet,std::vector<Eigen::Matrix3d>> _sample_orientations_ref;
   std::map<nsx::sptrDataSet,std::vector<Eigen::Matrix3d>> _detector_orientations_ref;
   std::map<nsx::sptrDataSet,std::vector<Eigen::Vector3d>> _sample_positions_ref;
   std::map<nsx::sptrDataSet,std::vector<Eigen::Vector3d>> _detector_positions_ref;
   std::map<nsx::sptrDataSet,std::vector<Eigen::Vector3d>> _nis_ref;
   std::map<nsx::sptrDataSet,std::vector<double>> _wavelengths_ref;
   std::map<nsx::sptrDataSet,std::vector<double>> _wavelength_offsets_ref;

   std::map<nsx::sptrDataSet,Eigen::Vector3d> _uc_dims_ref;
   std::map<nsx::sptrDataSet,Eigen::Vector3d> _uc_angles_ref;

   int _current_frame;

   std::map<nsx::sptrDataSet, nsx::Refiner> refiners;
   std::vector<std::string> names_plotable;

private:

   QVBoxLayout* _input_layout;
   QVBoxLayout* _info_layout;
   QVBoxLayout* _graph_layout;

   QComboBox* _select_uc;
   QComboBox* _select_data;

   QListWidget* _select_peaks_list;
   QListWidget* _select_data_list;

   QCheckBox* _refine_lattice;
   QCheckBox* _refine_sample_position;
   QCheckBox* _refine_detector_position;
   QCheckBox* _refine_sample_orientation;
   QCheckBox* _refine_ki;
   QSpinBox* _number_of_batches;
   QPushButton* _refine;

   QScienceSpinBox* _ni_X;
   QScienceSpinBox* _ni_Y;
   QScienceSpinBox* _ni_Z;
   QScienceSpinBox* _wavelength;
   QScienceSpinBox* _wavelength_offset;

   QScienceSpinBox* _ni_X_ref;
   QScienceSpinBox* _ni_Y_ref;
   QScienceSpinBox* _ni_Z_ref;
   QScienceSpinBox* _wavelength_ref;
   QScienceSpinBox* _wavelength_offset_ref;

   QScienceSpinBox* _uc_a;
   QScienceSpinBox* _uc_b;
   QScienceSpinBox* _uc_c;
   QScienceSpinBox* _uc_alpha;
   QScienceSpinBox* _uc_beta;
   QScienceSpinBox* _uc_gamma;

   QScienceSpinBox* _uc_a_ref;
   QScienceSpinBox* _uc_b_ref;
   QScienceSpinBox* _uc_c_ref;
   QScienceSpinBox* _uc_alpha_ref;
   QScienceSpinBox* _uc_beta_ref;
   QScienceSpinBox* _uc_gamma_ref;

   QScienceSpinBox* _sample_position_X;
   QScienceSpinBox* _sample_position_Y;
   QScienceSpinBox* _sample_position_Z;

   QScienceSpinBox* _sample_position_X_ref;
   QScienceSpinBox* _sample_position_Y_ref;
   QScienceSpinBox* _sample_position_Z_ref;

   QScienceSpinBox* _sample_orientation_00;
   QScienceSpinBox* _sample_orientation_01;
   QScienceSpinBox* _sample_orientation_02;
   QScienceSpinBox* _sample_orientation_10;
   QScienceSpinBox* _sample_orientation_11;
   QScienceSpinBox* _sample_orientation_12;
   QScienceSpinBox* _sample_orientation_20;
   QScienceSpinBox* _sample_orientation_21;
   QScienceSpinBox* _sample_orientation_22;

   QScienceSpinBox* _sample_orientation_00_ref;
   QScienceSpinBox* _sample_orientation_01_ref;
   QScienceSpinBox* _sample_orientation_02_ref;
   QScienceSpinBox* _sample_orientation_10_ref;
   QScienceSpinBox* _sample_orientation_11_ref;
   QScienceSpinBox* _sample_orientation_12_ref;
   QScienceSpinBox* _sample_orientation_20_ref;
   QScienceSpinBox* _sample_orientation_21_ref;
   QScienceSpinBox* _sample_orientation_22_ref;

   QScienceSpinBox* _detector_position_X;
   QScienceSpinBox* _detector_position_Y;
   QScienceSpinBox* _detector_position_Z;

   QScienceSpinBox* _detector_position_X_ref;
   QScienceSpinBox* _detector_position_Y_ref;
   QScienceSpinBox* _detector_position_Z_ref;

   QScienceSpinBox* _detector_orientation_00;
   QScienceSpinBox* _detector_orientation_01;
   QScienceSpinBox* _detector_orientation_02;
   QScienceSpinBox* _detector_orientation_10;
   QScienceSpinBox* _detector_orientation_11;
   QScienceSpinBox* _detector_orientation_12;
   QScienceSpinBox* _detector_orientation_20;
   QScienceSpinBox* _detector_orientation_21;
   QScienceSpinBox* _detector_orientation_22;

   QScienceSpinBox* _detector_orientation_00_ref;
   QScienceSpinBox* _detector_orientation_01_ref;
   QScienceSpinBox* _detector_orientation_02_ref;
   QScienceSpinBox* _detector_orientation_10_ref;
   QScienceSpinBox* _detector_orientation_11_ref;
   QScienceSpinBox* _detector_orientation_12_ref;
   QScienceSpinBox* _detector_orientation_20_ref;
   QScienceSpinBox* _detector_orientation_21_ref;
   QScienceSpinBox* _detector_orientation_22_ref;

   QSizePolicy* _size_policy_widgets;
   QSizePolicy* _size_policy_box;
   QSizePolicy* _size_policy_right;
   QSizePolicy* _size_policy_fixed;

   QGridLayout* _sample_layout;
   QGridLayout* _uc_layout;
   QGridLayout* _instrument_layout;
   QGridLayout* _detector_layout;

   QWidget* _navigator;
   QPushButton* _left;
   QPushButton* _right;
   QSpinBox* _current_index_spin;

   SXPlot* _visualise_plot;

};

#endif // GUI_FRAMES_REFINERFRAME_H
