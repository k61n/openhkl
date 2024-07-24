//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_find/SubframeFindPeaks.h
//! @brief     Defines classes FoundPeaks, SubframeFindPeaks
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBFRAME_FIND_SUBFRAMEFINDPEAKS_H
#define OHKL_GUI_SUBFRAME_FIND_SUBFRAMEFINDPEAKS_H

#include "core/shape/PeakCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

#include <QWidget>

class DataComboBox;
class DetectorWidget;
class PeakItemGraphic;
class PeakTableView;
class PeakViewWidget;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QPushButton;
class QSplitter;
class QTableWidget;
class QVBoxLayout;
class SafeSpinBox;
class SafeDoubleSpinBox;

//! Frame containing interface to find peaks from detector images
class SubframeFindPeaks : public QWidget {
    Q_OBJECT
 public:
    SubframeFindPeaks();
    //! Change the convolution parameters
    void updateConvolutionParameters();
    //! Find peaks
    void find();
    //! integrate found peaks
    void integrate();
    //! Refresh all the panels
    void refreshAll();
    //! Get the detector widget
    DetectorWidget* detectorWidget();
    //! Grab the finder parameters
    void grabFinderParameters();
    //! Grab the integration parameters
    void grabIntegrationParameters();
    //! Show the filtered/thresholded image in DetectorScene
    void showFilteredImage();

    //! Set the finder parameters
    void setFinderParameters();
    //! Set the integration parameters
    void setIntegrationParameters();

 public slots:
    void onGradientSettingsChanged();

 signals:
    void signalGradient(int kernel, bool fft);

 private:
    //! Set up the data selection GUI
    void setDataUp();
    //! Set up the blob finding GUI
    void setBlobUp();
    //! Set up the Preview GUI
    void setPreviewUp();
    //! Set up the save button
    void setSaveUp();
    //! Set up the Preview GUI
    void setIntegrateUp();
    //! Set up the detector figure up
    void setFigureUp();
    //! Set the peak table view up
    void setPeakTableUp();

    //! Refresh the found peaks list
    void refreshPeakTable();
    //! Change the peak selected in the table
    void changeSelected(PeakItemGraphic* peak_graphic);
    //! Accept and save current list
    void accept();
    //! Disable widgets that are unsafe without relevant data
    void toggleUnsafeWidgets();

    //! Convolution parameter map
    std::map<std::string, double> convolutionParameters();

    //! The model for the found peaks
    ohkl::PeakCollection _peak_collection;
    //! The temporary collection
    PeakCollectionItem _peak_collection_item;
    //! The temporary collection
    PeakCollectionModel _peak_collection_model;

    //! Flag to check whether the peaks have been integrated
    bool _peaks_integrated;

    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    DataComboBox* _data_combo;

    SafeSpinBox* _threshold_spin;
    SafeDoubleSpinBox* _scale_spin;
    SafeSpinBox* _min_size_spin;
    SafeSpinBox* _max_size_spin;
    SafeSpinBox* _max_width_spin;
    QComboBox* _kernel_combo;
    QTableWidget* _kernel_para_table;
    SafeSpinBox* _start_frame_spin;
    SafeSpinBox* _end_frame_spin;

    PeakViewWidget* _peak_view_widget;
    QCheckBox* _threshold_check;

    QComboBox* _integration_region_type;
    SafeDoubleSpinBox* _peak_end;
    SafeDoubleSpinBox* _bkg_begin;
    SafeDoubleSpinBox* _bkg_end;
    SafeSpinBox* _max_width;
    QGroupBox* _use_max_width;
    QCheckBox* _gradient_check;
    QCheckBox* _fft_gradient_check;
    QComboBox* _gradient_kernel;

    QPushButton* _find_button;
    QPushButton* _integrate_button;
    QPushButton* _save_button;

    DetectorWidget* _detector_widget;
    QGraphicsPixmapItem* _pixmap;

    PeakTableView* _peak_table;

    const std::map<ohkl::GradientKernel, QString> _kernel_description{
        {ohkl::GradientKernel::CentralDifference, "Central difference"},
        {ohkl::GradientKernel::Sobel, "Sobel 3x3"},
        {ohkl::GradientKernel::Sobel5, "Sobel 5x5"},
        {ohkl::GradientKernel::Prewitt, "Prewitt"},
    };
};

#endif // OHKL_GUI_SUBFRAME_FIND_SUBFRAMEFINDPEAKS_H
