//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_reject/SubframeReject.h
//! @brief     Defines class SubframeReject
//!
//! @homepage  https://openhkl.org
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
    //! Set the horizontal range in the control widget
    void updateXRange(double xmin, double xmax);
    //! Set the vertical range in the control widget
    void updateYRange(double ymin, double ymax);
    //! When a peak is selected via the table
    void onPeakTableSelection();

 private:
    //! Select dataset, peak collection
    void setInputUp();
    //! Find peak by miller index
    void setFindUp();
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

    //! Refresh the peak table
    void refreshPeakTable();
    //! Scroll to selected peak in table
    void changeSelected(PeakItemGraphic* peak_graphic);
    //! Refresh the found peaks list
    void refreshTables();
    //! Disable unsafe widgets if no data loaded
    void toggleUnsafeWidgets();

    //! Find a peak by Miller index
    void findByIndex();

    //! Update the computed statistics
    void updateStatistics();
    //! Compute the selected histogram
    void computeHistogram();
    //! Update the plot range based on the selected peak collection
    void updatePlotRange();
    //! Reject outliers in given data field
    void rejectOutliers();

    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    // data selection
    PeakComboBox* _peak_combo;
    DataComboBox* _data_combo;

    // peak search
    SafeSpinBox* _find_h;
    SafeSpinBox* _find_k;
    SafeSpinBox* _find_l;
    QPushButton* _find_by_index;

    // histogram generation
    QComboBox* _histo_combo;
    SafeSpinBox* _n_bins;
    SafeSpinBox* _freq_min;
    SafeSpinBox* _freq_max;
    SafeSpinBox* _x_min;
    SafeSpinBox* _x_max;
    QCheckBox* _log_freq;
    QPushButton* _plot_histogram;
    SafeDoubleSpinBox* _sigma_factor;
    QPushButton* _reject_outliers;

    PeakViewWidget* _peak_view_widget;
    DetectorWidget* _detector_widget;
    PlotPanel* _plot_widget;

    PeakTableView* _peak_table;
    ohkl::PeakCollection* _peak_collection;
    PeakCollectionItem _peak_collection_item;
    PeakCollectionModel _peak_collection_model;
    ohkl::PeakStatistics _peak_stats;

    gsl_histogram* _current_histogram;

    std::vector<PeakItemGraphic*> _selected_graphics;
    QColor _selection_color;
    QVector<QColor> _saved_colors;
};


#endif // OHKL_GUI_SUBFRAME_REJECT_SUBFRAMEREJECT_H
