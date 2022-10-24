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

#include "core/convolve/Convolver.h"
#include "core/data/DataTypes.h"
#include "core/detector/DetectorEvent.h"
#include "gui/utility/SafeSpinBox.h"
#include "tables/crystal/UnitCell.h"
#include "gui/models/MaskHandler.h"

#include <QWidget>

class DataComboBox;
class DetectorWidget;
class PlotPanel;
class PropertyPanel;
class QGraphicsPixmapItem;
class QPushButton;
class QCheckBox;
class QComboBox;
class QSlider;
class QTabWidget;
class QTableWidget;
class QVBoxLayout;
class QHBoxLayout;
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

    DetectorWidget* detectorWidget();

    void refreshVisual();
    void refreshAll();

    PlotPanel* getPlot() { return _plot; };
    PropertyPanel* getProperty() { return _properties; };

    void find_2d();
    void autoindex();
    void plotIntensities();
    void toggleUnsafeWidgets();
    void updateRanges();
    void showFilteredImage();

    int getNumberDataPoints() {return _npoints_lineplot->value();};
    void calculateIntensities();
    void setLogarithmicScale();

    void grabFinderParameters();
    void setFinderParameters();

    void grabIndexerParameters();
    void setIndexerParameters();

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

    void refreshMaskTable();

    void setPlotMode();
    void setMaskMode();
   //! exports masks to yaml file
    void exportMasks();
   //! imports maks from yaml file
    void importMasks();
   //! deletes selected masks from dataset
    void deleteSelectedMasks();
    void selectAllMasks();

 public slots:
    void onBeamPosChanged(QPointF pos);
    void onBeamPosSpinChanged();
    void onMaskChanged();
    void onMaskSelected();

 signals:
    void beamPosChanged(QPointF pos);
    void crosshairChanged(int size, int linewidth);

 private:
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
    QCheckBox* _xZoom;
    QCheckBox* _yZoom;
    QSpinBox*  _npoints_intensity;
    QSpinBox* _npoints_lineplot;
    QSpinBox* _minX;
    QSpinBox* _maxX;
    QSpinBox* _minY;
    QSpinBox* _maxY;

    SpoilerCheck* _set_initial_ki;
    QCheckBox* _direct_beam;
    SafeDoubleSpinBox* _beam_offset_x;
    SafeDoubleSpinBox* _beam_offset_y;
    QSlider* _crosshair_size;
    SafeSpinBox* _crosshair_linewidth;

    DataComboBox* _data_combo;
    QComboBox* _convolver_combo;
    SafeSpinBox* _threshold;
    SafeSpinBox* _blob_min_thresh;
    SafeSpinBox* _blob_max_thresh;
    QCheckBox* _search_all_frames;
    QCheckBox* _threshold_check;
    QPushButton* _find_peaks_2d;

    SafeDoubleSpinBox* _gruber;
    SafeDoubleSpinBox* _niggli;
    SafeDoubleSpinBox* _max_cell_dimension;
    SafeDoubleSpinBox* _min_cell_volume;
    SafeDoubleSpinBox* _indexing_tolerance;
    SafeDoubleSpinBox* _frequency_tolerance;
    SafeSpinBox* _number_vertices;
    SafeSpinBox* _number_solutions;
    SafeSpinBox* _number_subdivisions;
    QCheckBox* _only_niggli;
    QPushButton* _index_button;
    QPushButton* _save_button;

    UnitCellTableView* _solution_table;

    SpoilerCheck* _lineplot_box;
    Spoiler* _intensity_plot_box;
    QComboBox* _lineplot_combo;

    SpoilerCheck* _mask_box;
    QComboBox* _mask_combo;
    QTableWidget* _mask_table;

    QPushButton* _export_masks;
    QPushButton* _import_masks;
    QPushButton* _delete_masks;
    QPushButton* _toggle_selection;

    bool _show_direct_beam;

    //! Saved direct beam positions
    std::vector<ohkl::DetectorEvent> _direct_beam_events;
    //! Current direct beam positions
    std::vector<ohkl::DetectorEvent> _old_direct_beam_events;

    //! Storage of indexer solutions
    std::vector<std::pair<std::shared_ptr<ohkl::UnitCell>, double>> _solutions;
    //! Unit cell selected in solution table
    ohkl::sptrUnitCell _selected_unit_cell;
    //! Filtered/thresholded image
    QGraphicsPixmapItem* _thresholded_image;

    std::shared_ptr<MaskHandler> _mask_handler;
};

#endif // OHKL_GUI_SUBFRAME_EXPERIMENT_SUBFRAMEEXPERIMENT_H
