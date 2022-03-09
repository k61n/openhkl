//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_home/SubframeHome.h
//! @brief     Defines class SubframeHome
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBFRAME_HOME_SUBFRAMEHOME_H
#define NSX_GUI_SUBFRAME_HOME_SUBFRAMEHOME_H

#include "gui/models/ExperimentModel.h"
#include "gui/views/ExperimentTableView.h"

#include <memory>

#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QTableWidget>

//! Frame containing interface to create, save and load experiments
class SubframeHome : public QWidget {
 public:
    //! Default constructor
    SubframeHome();

 private:
    //! Set the left layout
    void _setLeftLayout(QHBoxLayout* main_layout);
    //! Set the right layout
    void _setRightLayout(QHBoxLayout* main_layout);

 private:
    //! Switch the current experiment through the model index
    void _switchCurrentExperiment(const QModelIndex& index) const;
    //! Update the current list
    void _updateLastLoadedList(QString name, QString file_path);
    //! Update the current list widget
    void _updateLastLoadedWidget();
    //! Load the clicked item
    void _loadSelectedItem(QListWidgetItem* item);

 public:
    //! Create a new experiment
    void createNew();
    //! Load an experiment from file
    void loadFromFile();

    //! Save the current experiment
    void saveCurrent();
    //! Save all experiments provided they have save paths (not implemented)
    void saveAll();

    //! Read the settings
    void readSettings();
    // ! Save the settings
    void saveSettings() const;

    //! Disable unsafe widgets if no data loaded
    void toggleUnsafeWidgets();

    void refreshTables();

   // void UpdatePeakList();

 private:
    QPushButton* _new_exp;
    QPushButton* _old_exp;
    QPushButton* _save_current;
    QPushButton* _save_all;

    ExperimentTableView* _open_experiments_view;
    std::unique_ptr<ExperimentModel> _open_experiments_model;

    QList<QStringList> _last_imports;
    QListWidget* _last_import_widget;

    QTableWidget* _dataset_table;
    QTableWidget* _peak_collections_table;
    QTableWidget* _unitcell_table;

};

#endif // NSX_GUI_SUBFRAME_HOME_SUBFRAMEHOME_H
