//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_experiment/SubframeExperiment.h
//! @brief     Defines class SubframeExperiment
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBFRAME_EXPERIMENT_SUBFRAMEEXPERIMENT_H
#define OHKL_GUI_SUBFRAME_EXPERIMENT_SUBFRAMEEXPERIMENT_H

#include "core/data/DataTypes.h"
#include "core/detector/DetectorEvent.h"
#include "core/shape/PeakCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"

#include <QSpinBox>
#include <QWidget>

class CellComboBox;
class DataComboBox;
class DetectorWidget;
class DirectBeamWidget;
class PeakViewWidget;
class PlotPanel;
class PropertyPanel;
class QPushButton;
class QCheckBox;
class QComboBox;
class QTabWidget;
class QTableWidget;
class QVBoxLayout;
class QHBoxLayout;
class SafeDoubleSpinBox;
class SafeSpinBox;
class Spoiler;
class SpoilerCheck;
class UnitCellTableView;

//! Frame containing information on all aspects of the experiment
class SubframeExperiment : public QWidget {
    Q_OBJECT
 public:
    SubframeExperiment();

    void setLeftWidgetUp();
    void setStrategyUp();
    void setHistogramUp();
    void setMaskUp();
    void setAdjustBeamUp();
    void setPeakFinder2DUp();
    void setIndexerUp();
    void setPredictUp();
    void setPreviewUp();

    DetectorWidget* detectorWidget();

    void refreshVisual();
    void refreshAll();

    PlotPanel* getPlot() { return _plot; };
    PropertyPanel* getProperty() { return _properties; };

    void find_2d();
    void autoindex();
    void predict();
    void merge();
    void savePeaks();
    void plotIntensities();
    void toggleUnsafeWidgets();
    void updateRanges();
    void showFilteredImage();

    int getNumberDataPoints() { return _npoints_lineplot->value(); };
    void calculateIntensities();
    void setLogarithmicScale();

    void grabFinderParameters();
    void setFinderParameters();

    void grabIndexerParameters();
    void setIndexerParameters();

    void grabStrategyParameters();
    void setStrategyParameters();

    //! Transmit crosshair changes to DetectorScene
    void changeCrosshair();
    //! Toggle cursor mode
    void toggleCursorMode();
    //! Set the initial value of ki from the crosshair position
    void setInitialKi(ohkl::sptrDataSet data);
    //! Show direct beam position computed from unit cell in DetectorScene
    void showDirectBeamEvents();

    //! Populate the indexer solutions table
    void buildSolutionTable();
    //! Select indexer solution
    void selectSolutionHeader(int index);
    //! Select a solution
    void selectSolutionTable();
    //! Save a unit cell to the experiment
    void saveCell();
    //! Set unit cell for displaying Miller index tooltip
    void setUnitCell();

    //! Update the mask table with correct bounds
    void refreshMaskTable();

    //! Change DetectorScene interaction mode to histogram plot
    void setPlotMode();
    //! Change DetectorScene interaction mode to draw masks
    void setMaskMode();

    //! exports masks to yaml file
    void exportMasks();
    //! imports maks from yaml file
    void importMasks();
    //! deletes selected masks from dataset
    void deleteSelectedMasks();
    //! Set all graphical mask items to selected
    void selectAllMasks();

 public slots:
    void onMaskChanged();
    void onMaskSelected();
    //! Reset DetectorScene interaction mode
    void resetMode(int index);

 private:
    void refreshPeaks();

    QTabWidget* _tab_widget;
    QTabWidget* _left_widget;
    DetectorWidget* _detector_widget;
    PlotPanel* _plot;
    PropertyPanel* _properties;
    QVBoxLayout* _strategy_layout;
    QVBoxLayout* _histogram_layout;
    QVBoxLayout* _mask_layout;
    QVBoxLayout* _left_layout;
    QHBoxLayout* _main_layout;

    QPushButton* _calc_intensity;
    QPushButton* _update_plot;
    QCheckBox* _totalHistogram;
    QCheckBox* _yLog;
    QSpinBox* _npoints_intensity;
    QSpinBox* _npoints_lineplot;
    QSpinBox* _minX;
    QSpinBox* _maxX;
    QSpinBox* _minY;
    QSpinBox* _maxY;

    // direct beam adjustment
    Spoiler* _set_initial_ki;
    DirectBeamWidget* _beam_setter_widget;

    // 2D peak finder
    DataComboBox* _data_combo;
    QComboBox* _convolver_combo;
    SafeSpinBox* _threshold_spin;
    SafeSpinBox* _blob_min_thresh;
    SafeSpinBox* _blob_max_thresh;
    QCheckBox* _search_all_frames;
    QCheckBox* _threshold_check;
    QPushButton* _find_peaks_2d;

    // index
    SafeDoubleSpinBox* _gruber;
    SafeDoubleSpinBox* _niggli;
    SafeDoubleSpinBox* _max_cell_dimension;
    SafeDoubleSpinBox* _min_cell_volume;
    SafeDoubleSpinBox* _indexing_tolerance;
    SafeDoubleSpinBox* _frequency_tolerance;
    SafeDoubleSpinBox* _d_min;
    SafeDoubleSpinBox* _d_max;
    SafeSpinBox* _number_vertices;
    SafeSpinBox* _number_solutions;
    SafeSpinBox* _number_subdivisions;
    QCheckBox* _only_niggli;
    QPushButton* _index_button;
    QPushButton* _save_cell;

    // predict
    CellComboBox* _cell_combo;
    SafeDoubleSpinBox* _delta_chi;
    SafeDoubleSpinBox* _delta_omega;
    SafeDoubleSpinBox* _delta_phi;
    SafeSpinBox* _n_increments;
    SafeDoubleSpinBox* _predict_d_min;
    SafeDoubleSpinBox* _predict_d_max;
    QPushButton* _predict_button;
    QPushButton* _save_peaks;


    UnitCellTableView* _solution_table;

    SpoilerCheck* _lineplot_box;
    Spoiler* _intensity_plot_box;
    QComboBox* _lineplot_combo;

    SpoilerCheck* _mask_box;
    QComboBox* _mask_combo;
    QTableWidget* _mask_table;
    int _mask_table_rows;

    QPushButton* _export_masks;
    QPushButton* _import_masks;
    QPushButton* _delete_masks;
    QPushButton* _toggle_selection;

    PeakViewWidget* _peak_view_widget;

    bool _show_direct_beam;

    //! Saved direct beam positions
    std::vector<ohkl::DetectorEvent> _direct_beam_events;
    //! Current direct beam positions
    std::vector<ohkl::DetectorEvent> _old_direct_beam_events;

    //! Storage of indexer solutions
    std::vector<std::pair<std::shared_ptr<ohkl::UnitCell>, double>> _solutions;
    //! Unit cell selected in solution table
    ohkl::sptrUnitCell _selected_unit_cell;

    //! Strategy predicted peaks
    ohkl::PeakCollection _peak_collection;
    PeakCollectionItem _peak_collection_item;
    PeakCollectionModel _peak_collection_model;
};

#endif // OHKL_GUI_SUBFRAME_EXPERIMENT_SUBFRAMEEXPERIMENT_H
