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

#include <QPushButton>
#include <QSizePolicy>
#include <QWidget>

class LinkedComboBox;
class PeakTableView;
class UnitCellTableView;
class Spoiler;
class QVBoxLayout;
class QSplitter;
class QSpinBox;
class QCheckBox;
class QDoubleSpinBox;

//! Frame containing interface to autoindex peak collections
class SubframeAutoIndexer : public QWidget {
 public:
    SubframeAutoIndexer();

    //! run the auto indexing
    void runAutoIndexer();
    //! Refresh all the panels
    void refreshAll();

 private:
    //! Build the input
    void setInputUp();
    //! Set the parameters values up
    void setParametersUp();
    //! Build the buttons
    void setProceedUp();
    //! Set the peak table view up
    void setPeakTableUp();
    //! Set the peak table view up
    void setSolutionTableUp();

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

    //! The model for the found peaks
    nsx::PeakCollection _peak_collection;
    //! The temporary collection
    PeakCollectionItem _peak_collection_item;
    //! The temporary collection
    PeakCollectionModel _peak_collection_model;

    std::vector<std::pair<nsx::sptrPeak3D, std::shared_ptr<const nsx::UnitCell>>> _defaults;
    std::vector<std::pair<std::shared_ptr<nsx::UnitCell>, double>> _solutions;

    nsx::sptrUnitCell _selected_unit_cell;

    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    LinkedComboBox* _exp_combo;
    LinkedComboBox* _data_combo;
    LinkedComboBox* _peak_combo;

    QSpinBox* _min_frame;
    QSpinBox* _max_frame;
    QDoubleSpinBox* _d_min;
    QDoubleSpinBox* _d_max;
    QDoubleSpinBox* _str_min;
    QDoubleSpinBox* _str_max;
    QDoubleSpinBox* _gruber;
    QDoubleSpinBox* _niggli;
    QDoubleSpinBox* _max_cell_dimension;
    QDoubleSpinBox* _min_cell_volume;
    QDoubleSpinBox* _indexing_tolerance;
    QDoubleSpinBox* _frequency_tolerance;

    QSpinBox* _number_vertices;
    QSpinBox* _number_solutions;
    QSpinBox* _number_subdivisions;

    QCheckBox* _only_niggli;

    QPushButton* _solve_button;
    QPushButton* _save_button;


    QSizePolicy _size_policy_right;

    PeakTableView* _peak_table;
    UnitCellTableView* _solution_table;
};

#endif // NSX_GUI_SUBFRAME_INDEX_SUBFRAMEAUTOINDEXER_H
