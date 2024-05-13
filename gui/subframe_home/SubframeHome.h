//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_home/SubframeHome.h
//! @brief     Defines class SubframeHome
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBFRAME_HOME_SUBFRAMEHOME_H
#define OHKL_GUI_SUBFRAME_HOME_SUBFRAMEHOME_H

#include <memory>

#include <QPair>
#include <QWidget>

class ExperimentModel;
class ExperimentTableView;
class InputFilesWindow;
class QComboBox;
class QHBoxLayout;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QTableWidget;

//! Frame containing interface to create, save and load experiments
class SubframeHome : public QWidget {
 public:
    //! Default constructor
    SubframeHome();

    //! Create a new experiment
    void createNew(bool strategy = false);
    //! Load an experiment from file
    void loadFromFile();

    //! Save the current experiment
    void saveCurrent(bool dialogue = false);
    //! Save all experiments provided they have save paths (not implemented)
    void saveAll();
    //! Remove the current experiment
    void removeCurrent();

    //! Read the settings
    void readSettings();
    // ! Save the settings
    void saveSettings() const;

    //! Disable unsafe widgets if no data loaded
    void toggleUnsafeWidgets();
    //! Refreshing tables on SubframeHome
    void refreshTables() const;
    //! Clearing tables on SubframeHome
    void clearTables();
    //! Setup ContextMenu on Dataset table
    void setContextMenuDatasetTable(QPoint pos);
    //! Setup ContextMenu on PeakCollection table
    void setContextMenuPeakTable(QPoint pos);
    //! Setup ContextMenu on UnitCell table
    void setContextMenuUnitCellTable(QPoint pos);

 public slots:
    //! Add a DataSet to the current experiment
    void addDataSet(int index);
    //! Add a single image to the current strategy experiment
    void addSingleImage(int index);

 private:
    //! Set the left layout
    void setLeftLayout(QHBoxLayout* main_layout);
    //! Set the right layout
    void setRightLayout(QHBoxLayout* main_layout);

    //! Switch the current experiment through the model index
    void switchCurrentExperiment(const QModelIndex& index);
    //! Update the current list
    void updateLastLoadedList(QString name, QString file_path);
    //! Update the current list widget
    void updateLastLoadedWidget();
    //! Load the clicked item
    void loadSelectedItem(QListWidgetItem* item);

    QPushButton* _new_exp;
    QPushButton* _new_strategy;
    QPushButton* _old_exp;
    QPushButton* _save_current;
    QPushButton* _save_all;
    QPushButton* _remove_current;
    QComboBox* _add_data;
    QComboBox* _add_single_image;

    ExperimentTableView* _open_experiments_view;
    std::unique_ptr<ExperimentModel> _open_experiments_model;

    QList<QPair<QString, QString>> _last_experiments;
    QListWidget* _last_import_widget;

    QTableWidget* _dataset_table;
    QTableWidget* _peak_collections_table;
    QTableWidget* _unitcell_table;
};

#endif // OHKL_GUI_SUBFRAME_HOME_SUBFRAMEHOME_H
