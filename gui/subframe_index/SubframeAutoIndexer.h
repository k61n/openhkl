//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_index/SubframeAutoIndexer.h
//! @brief     Defines class SubframeAutoIndexer
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_INDEX_SUBFRAMEAUTOINDEXER_H
#define NSX_GUI_SUBFRAME_INDEX_SUBFRAMEAUTOINDEXER_H

#include "core/algo/AutoIndexer.h"
#include "core/shape/PeakCollection.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/widgets/PeakViewWidget.h"

#include <QPushButton>
#include <QSizePolicy>
#include <QWidget>
#include <qglobal.h>
#include <qgridlayout.h>
#include <qgroupbox.h>
#include <qobjectdefs.h>

class DetectorWidget;
class LinkedComboBox;
class PeakTableView;
class UnitCellTableView;
class Spoiler;
class SpoilerCheck;
class SafeSpinBox;
class SafeDoubleSpinBox;
class QVBoxLayout;
class QSplitter;
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

 public slots:
    void onBeamPosChanged(QPointF pos);
    void onBeamPosSpinChanged();

 signals:
    void beamPosChanged(QPointF pos);
    void crosshairChanged(int size, int linewidth);

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
    //! Set the experiments
    void setExperiments();
    //! Update the list of data sets
    void updateDatasetList();
    //! Update the peak list
    void updatePeakList();
    //! Refresh the peak table
    void refreshPeakTable();
    //! Change the peak selected in the table
    void changeSelected(PeakItemGraphic* peak_graphic);
        //! Refresh the detector scene
        void refreshPeakVisual();
    //! Get the parameters of the indexer
    void grabIndexerParameters();
    //! Get the parameters of the indexer
    void setIndexerParameters();
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
    //! Set the initial value of ki from the crosshair position
    void setInitialKi(nsx::sptrDataSet data);
    //! Show direct beam position computed from unit cell in DetectorScene
    void showDirectBeamEvents();


    //! The model for the indexing peaks
    nsx::PeakCollection _peak_collection;
    //! The temporary collection
    PeakCollectionItem _peak_collection_item;
    //! The temporary collection
    PeakCollectionModel _peak_collection_model;
    //! List of data sets
    std::vector<nsx::sptrDataSet> _data_list;

    std::vector<std::pair<nsx::sptrPeak3D, std::shared_ptr<const nsx::UnitCell>>> _defaults;
    std::vector<std::pair<std::shared_ptr<nsx::UnitCell>, double>> _solutions;

    bool _show_direct_beam;

    nsx::sptrUnitCell _selected_unit_cell;

    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    LinkedComboBox* _exp_combo;
    LinkedComboBox* _data_combo;
    LinkedComboBox* _peak_combo;

    SpoilerCheck* _set_initial_ki;
    QCheckBox* _direct_beam;
    SafeDoubleSpinBox* _beam_offset_x;
    SafeDoubleSpinBox* _beam_offset_y;
    QSlider* _crosshair_size;
    SafeSpinBox* _crosshair_linewidth;

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
    std::vector<nsx::DetectorEvent> _direct_beam_events;
    //! Current direct beam positions
    std::vector<nsx::DetectorEvent> _old_direct_beam_events;
};

#endif // NSX_GUI_SUBFRAME_INDEX_SUBFRAMEAUTOINDEXER_H
