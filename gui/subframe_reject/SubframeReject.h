//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_refiner/SubframeReject.h
//! @brief     Defines class SubframeReject
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBFRAME_REJECT_SUBFRAMEREJECT_H
#define OHKL_GUI_SUBFRAME_REJECT_SUBFRAMEREJECT_H

#include "core/data/DataSet.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/PeakStatistics.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

#include <gsl/gsl_histogram.h>

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSizePolicy>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <qcombobox.h>
#include <qobjectdefs.h>
#include <qpushbutton.h>

class PeakComboBox;
class PlotPanel;
class FoundPeakComboBox;
class DataComboBox;
class DetectorWidget;
class PeakItemGraphic;
class PeakTableView;
class PeakViewWidget;
class Spoiler;
class SafeSpinBox;
class SafeDoubleSpinBox;

//! Frame containing interface to integrate peaks
class SubframeReject : public QWidget {
    Q_OBJECT
 public:
    SubframeReject();

    //! Refresh all the inputs
    void refreshAll();
    //! detector view
    DetectorWidget* detectorWidget();

 public slots:
    //! Filter the peak collection given a data range
    void filterSelection(double xmin, double xmax);

 private:
    //! Select dataset, peak collection
    void setInputUp();
    //! Input parameters for the histogram
    void setHistogramUp();

    //! Set up the peak view widget
    void setPreviewUp();
    //! Set up the DetectorScene
    void setFigureUp();
    //! Set up the plot widget
    void setPlotUp();
    //! Set up the peak table
    void setPeakTableUp();

    //! Refresh the DetctorScene
    void refreshPeakVisual();
    //! Refresh the peak table
    void refreshPeakTable();
    //! Scroll to selected peak in table
    void changeSelected(PeakItemGraphic* peak_graphic);
    //! Refresh the found peaks list
    void refreshTables();
    //! Disable unsafe widgets if no data loaded
    void toggleUnsafeWidgets();

    //! Compute the selected histogram
    void computeHistogram();
    //! Filter peaks based on selection in plot widget

    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    // data selection
    PeakComboBox* _peak_combo;
    DataComboBox* _data_combo;

    // histogram generation
    QComboBox* _histo_combo;
    SafeSpinBox* _n_bins;
    SafeSpinBox* _freq_min;
    SafeSpinBox* _freq_max;
    SafeSpinBox* _x_min;
    SafeSpinBox* _x_max;
    QCheckBox* _log_freq;
    QPushButton* _plot_histogram;

    PeakViewWidget* _peak_view_widget;
    DetectorWidget* _detector_widget;
    PlotPanel* _plot_widget;

    PeakTableView* _peak_table;
    ohkl::PeakCollection* _peak_collection;
    PeakCollectionItem _peak_collection_item;
    PeakCollectionModel _peak_collection_model;
    ohkl::PeakStatistics _peak_stats;

    gsl_histogram* _current_histogram;
};


#endif // OHKL_GUI_SUBFRAME_REJECT_SUBFRAMEREJECT_H
