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

#include "core/shape/PeakCollection.h"

#include "gui/graphics/DetectorView.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/Spoiler.h"
#include "gui/views/PeakTableView.h"
#include "gui/views/UnitCellTableView.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

//! Frame which shows the settings for the AutoIndexer
class SubframeAutoIndexer : public QWidget {
 public:
    SubframeAutoIndexer();
    ~SubframeAutoIndexer();
    //! run the auto indexing
    void runAutoIndexer();
    //! Refresh all the panels
    void refreshAll();

 private:
    //! Set up the GUI size policies
    void setSizePolicies();
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

 private:
    //! Build the table of solution
    void buildSolutionsTable();
    //! Set the experiments
    void setExperiments();
    //! Update the peak list
    void updatePeakList();
    //! Refresh the peak table
    void refreshPeakTable();
    //! Get the parameters of the indexer
    void grabIndexerParameters();
    //! Get the parameters of the indexer
    void setIndexerParameters() const;
    //! Select a solution
    void selectSolutionHeader(int index);
    //! Select a solution
    void selectSolutionTable();
    //! Accept the selected solution
    void acceptSolution();

 private:
    //! The model for the found peaks
    nsx::PeakCollection _peak_collection;
    //! The temporary collection
    PeakCollectionItem _peak_collection_item;
    //! The temporary collection
    PeakCollectionModel _peak_collection_model;
    //! The loaded peak list
    QStringList _peak_list;

    std::vector<std::pair<nsx::sptrPeak3D, std::shared_ptr<const nsx::UnitCell>>> _defaults;
    std::vector<std::pair<std::shared_ptr<nsx::UnitCell>, double>> _solutions;

    nsx::sptrUnitCell _selected_unit_cell;

 private:
    QHBoxLayout* _main_layout;
    QVBoxLayout* _left_layout;
    QSplitter* _right_element;

    Spoiler* _input_box;
    Spoiler* _para_box;

    QComboBox* _exp_combo;
    QComboBox* _peak_combo;

    QDoubleSpinBox* _gruber;
    QDoubleSpinBox* _niggli;
    QDoubleSpinBox* _max_cell_dimension;
    QDoubleSpinBox* _min_cell_volume;
    QDoubleSpinBox* _indexing_tolerance;

    QSpinBox* _number_vertices;
    QSpinBox* _number_solutions;
    QSpinBox* _number_subdivisions;

    QCheckBox* _only_niggli;

    QPushButton* _solve_button;
    QPushButton* _save_button;

    QSizePolicy* _size_policy_widgets;
    QSizePolicy* _size_policy_box;
    QSizePolicy* _size_policy_right;
    QSizePolicy* _size_policy_fixed;

    PeaksTableView* _peak_table;
    UnitCellTableView* _solution_table;
};

#endif // NSX_GUI_SUBFRAME_INDEX_SUBFRAMEAUTOINDEXER_H
