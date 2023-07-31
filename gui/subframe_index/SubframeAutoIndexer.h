//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_index/SubframeAutoIndexer.h
//! @brief     Defines class SubframeAutoIndexer
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBFRAME_INDEX_SUBFRAMEAUTOINDEXER_H
#define OHKL_GUI_SUBFRAME_INDEX_SUBFRAMEAUTOINDEXER_H

#include "core/algo/AutoIndexer.h"
#include "core/shape/PeakCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/widgets/PeakViewWidget.h"

#include <QPushButton>
#include <QSizePolicy>
#include <QWidget>

class CellComboBox;
class DataComboBox;
class DirectBeamWidget;
class FoundPeakComboBox;
class PeakComboBox;
class DetectorWidget;
class PeakTableView;
class UnitCellTableView;
class Spoiler;
class SpoilerCheck;
class SafeSpinBox;
class SafeDoubleSpinBox;
class QVBoxLayout;
class QSlider;
class QSplitter;
class QTabWidget;
class QCheckBox;
class QGroupBox;

//! Frame containing interface to autoindex peak collections
class SubframeAutoIndexer : public QWidget {
    Q_OBJECT
 public:
    SubframeAutoIndexer();

    //! run the auto indexing
    void runAutoIndexer();
    //! Refresh all the panels
    void refreshAll();
    //! Get the parameters of the indexer
    void grabIndexerParameters();
    //! Set the parameters of the indexer
    void setIndexerParameters();

 private:
    //! Build the input
    void setInputUp();
    //! Manually set the incident wavevector
    void setAdjustBeamUp();
    //! Set the parameters values up
    void setParametersUp();
    //! Build the buttons
    void setProceedUp();
    //! Set the peak table view up
    void setPeakTableUp();
    //! Set the peak table view up
    void setSolutionTableUp();
    //! Set peak view widget update
    void setPeakViewWidgetUp();
    //! Set up the detector scene
    void setFigureUp();

    //! Build the table of solution
    void buildSolutionsTable();
    //! Refresh the peak table
    void refreshPeakTable();
    //! Change the peak selected in the table
    void changeSelected(PeakItemGraphic* peak_graphic);
    //! Refresh the detector scene
    void refreshPeakVisual();
    //! Select a solution
    void selectSolutionHeader(int index);
    //! Select a solution
    void selectSolutionTable();
    //! Accept the selected solution
    void acceptSolution();
    //! Disable widgets that are unsafe without relevant data
    void toggleUnsafeWidgets();
    //! Transmit crosshair changes to DetectorScene
    void changeCrosshair();
    //! Toggle cursor mode
    void toggleCursorMode();
    //! Show direct beam position computed from unit cell in DetectorScene
    void showDirectBeamEvents();

    //! The model for the indexing peaks
    ohkl::PeakCollection _peak_collection;
    //! The temporary collection
    PeakCollectionItem _peak_collection_item;
    //! The temporary collection
    PeakCollectionModel _peak_collection_model;

    std::vector<std::pair<ohkl::sptrPeak3D, std::shared_ptr<const ohkl::UnitCell>>> _defaults;
    std::vector<std::pair<std::shared_ptr<ohkl::UnitCell>, double>> _solutions;

    bool _show_direct_beam;

    ohkl::sptrUnitCell _selected_unit_cell;

    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    QTabWidget* _tab_widget;

    DataComboBox* _data_combo;
    PeakComboBox* _peak_combo;

    Spoiler* _set_initial_ki;
    DirectBeamWidget* _beam_setter_widget;

    SafeSpinBox* _min_frame;
    SafeSpinBox* _max_frame;
    SafeDoubleSpinBox* _d_min;
    SafeDoubleSpinBox* _d_max;
    SafeDoubleSpinBox* _str_min;
    SafeDoubleSpinBox* _str_max;
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

    QPushButton* _solve_button;
    QPushButton* _save_button;

    QSizePolicy _size_policy_right;

    QGroupBox* _peak_group;
    QVBoxLayout* _solution_layout;
    PeakTableView* _peak_table;
    UnitCellTableView* _solution_table;

    DetectorWidget* _detector_widget;

    PeakViewWidget* _peak_view_widget;

    int _stored_cursor_mode;

    //! Saved direct beam positions
    std::vector<ohkl::DetectorEvent> _direct_beam_events;
    //! Current direct beam positions
    std::vector<ohkl::DetectorEvent> _old_direct_beam_events;
};

#endif // OHKL_GUI_SUBFRAME_INDEX_SUBFRAMEAUTOINDEXER_H
