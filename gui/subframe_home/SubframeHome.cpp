//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_home/SubframeHome.cpp
//! @brief     Implements class SubframeHome
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_home/SubframeHome.h"

#include "base/utils/Units.h"
#include "core/experiment/Experiment.h"
#include "core/shape/PeakCollection.h"
#include "gui/MainWin.h"
#include "gui/dialogs/ExperimentDialog.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "tables/crystal/UnitCell.h"
#include "gui/utility/SideBar.h"
#include "gui/subwindows/AxisWindow.h"
#include "core/data/DataTypes.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSpacerItem>
#include <QHeaderView>
#include <QListView>
#include <QStringListModel>
#include <QAbstractItemModel>
#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>
#include <unistd.h>

// Icon attributions:
// save.svg: folder open by Loudoun Design Co from the Noun Project
// delete.svg: trash by Loudoun Design Co from the Noun Project
// open.svg: open book by Loudoun Design Co from the Noun Project
// plus.svg: circle plus by Loudoun Design Co from the Noun Project
// minus.svg: circle minus by Loudoun Design Co from the Noun Project
// beaker.svg: beaker by Loudoun Design Co from the Noun Project



SubframeHome::SubframeHome()
{
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    QSpacerItem* spacer_top = new QSpacerItem(10, 50, QSizePolicy::Minimum, QSizePolicy::Fixed);
    main_layout->addSpacerItem(spacer_top);

    QHBoxLayout* sub_layout = new QHBoxLayout();
    QSpacerItem* spacer_left = new QSpacerItem(50, 10, QSizePolicy::Fixed, QSizePolicy::Minimum);
    sub_layout->addSpacerItem(spacer_left);

    _setLeftLayout(sub_layout);
    _setRightLayout(sub_layout);

    // QSpacerItem* spacer_right =
    //     new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Minimum);
    // sub_layout->addSpacerItem(spacer_right);
    main_layout->addLayout(sub_layout);

    QSpacerItem* spacer_bot = new QSpacerItem(10, 50, QSizePolicy::Minimum, QSizePolicy::Fixed);
    main_layout->addSpacerItem(spacer_bot);

    readSettings();
    toggleUnsafeWidgets();
    _updateLastLoadedWidget();
    gGui->refreshMenu();
}

void SubframeHome::_setLeftLayout(QHBoxLayout* main_layout)
{
    QVBoxLayout* left = new QVBoxLayout;
    QHBoxLayout* left_top = new QHBoxLayout();
    QString tooltip;

    QString path{":images/icons/"};
    QString light{"lighttheme/"};
    QString dark{"darktheme/"};

    if (gGui->isDark()) // looks like we have a dark theme
        path = path + dark;
    else
        path = path + light;

    _new_exp = new QPushButton();
    _new_exp->setIcon(QIcon(path + "plus.svg"));
    _new_exp->setText("Create new experiment");
    _new_exp->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    connect(_new_exp, &QPushButton::clicked, this, &SubframeHome::createNew);

    _old_exp = new QPushButton();
    _old_exp->setIcon(QIcon(path + "open.svg"));
    _old_exp->setText("Load from file");
    _old_exp->setMinimumWidth(_new_exp->sizeHint().width());
    _old_exp->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    tooltip = "Load an existing experiment from a .nsx (HDF5) file";
    _old_exp->setToolTip(tooltip);
    connect(_old_exp, &QPushButton::clicked, this, &SubframeHome::loadFromFile);

    left_top->addWidget(_new_exp);
    left_top->addWidget(_old_exp);

    left->addLayout(left_top);

    _last_import_widget = new QListWidget(this);
     
    _last_import_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _last_import_widget->setStyleSheet("background-color: transparent;");
    connect(_last_import_widget, &QListWidget::itemClicked, this, &SubframeHome::_loadSelectedItem);

    left->addWidget(_last_import_widget);

    //QSpacerItem* spacer_bottom =
    //    new QSpacerItem(10, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    //left->addSpacerItem(spacer_bottom);

    _dataset_table = new QTableWidget(0, 7);
    _dataset_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _dataset_table->setHorizontalHeaderLabels(QStringList{
        "Name", "Diffractometer", "Number of Frames", "Number of Cols", 
        "Number of Rows","Distance/A", "Wavelength/A",
        });
    _dataset_table->resizeColumnsToContents();
    _dataset_table->verticalHeader()->setVisible(false);
    _dataset_table->setContextMenuPolicy(Qt::CustomContextMenu);

    _peak_collections_table = new QTableWidget(0, 6);
    _peak_collections_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _peak_collections_table->setHorizontalHeaderLabels(QStringList{
        "Name", "Number of Peaks", "Number of Invalid Peaks", "Is indexed", "Is integrated",
        "Type"});
    _peak_collections_table->resizeColumnsToContents();
    _peak_collections_table->verticalHeader()->setVisible(false);
    _peak_collections_table->setContextMenuPolicy(Qt::CustomContextMenu);

    _unitcell_table = new QTableWidget(0, 9);
    _unitcell_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _unitcell_table->setContextMenuPolicy(Qt::CustomContextMenu);
    _unitcell_table->setHorizontalHeaderLabels(QStringList{
        "ID", "Name", "Space Group", "a", "b", "c", QChar(0xb1, 0x03), QChar(0xb2, 0x03),
        QChar(0xb3, 0x03)
        });
    _unitcell_table->resizeColumnsToContents();
    _unitcell_table->verticalHeader()->setVisible(false);

    // labels for tables
    QLabel* lab_dataset = new QLabel("Datasets", this);
    QLabel* lab_peaks = new QLabel("Peak Collections", this);
    QLabel* lab_unitcell = new QLabel("Unit Cells", this);

    QHBoxLayout* lay_datasets_head = new QHBoxLayout();
    QVBoxLayout* lay_datasets = new QVBoxLayout();
    QHBoxLayout* lay_peaks_head = new QHBoxLayout();
    QVBoxLayout* lay_peaks = new QVBoxLayout();
    QVBoxLayout* lay_unitcells = new QVBoxLayout();

    lay_datasets_head->addWidget(lab_dataset);

    lay_datasets->addLayout(lay_datasets_head);
    lay_datasets->addWidget(_dataset_table);

    lay_peaks_head->addWidget(lab_peaks);
    lay_peaks->addLayout(lay_peaks_head);
    lay_peaks->addWidget(_peak_collections_table);

    lay_unitcells->addWidget(lab_unitcell);
    lay_unitcells->addWidget(_unitcell_table);

    left->addLayout(lay_datasets);
    left->addLayout(lay_peaks);
    left->addLayout(lay_unitcells);

    refreshTables();

    main_layout->addLayout(left); 

    connect(_dataset_table, &QWidget::customContextMenuRequested, this, &SubframeHome::setContextMenuDatasetTable);
    connect(_peak_collections_table, &QWidget::customContextMenuRequested, this, &SubframeHome::setContextMenuPeakTable);
    connect( _unitcell_table, &QWidget::customContextMenuRequested, this, &SubframeHome::setContextMenuUnitCellTable);    
}

void SubframeHome::_setRightLayout(QHBoxLayout* main_layout)
{
    QVBoxLayout* right = new QVBoxLayout;

    _open_experiments_model = std::make_unique<ExperimentModel>();
    _open_experiments_view = new ExperimentTableView();
    _open_experiments_view->setModel(_open_experiments_model.get());
    connect(
        _open_experiments_view, &ExperimentTableView::clicked, this,
        &SubframeHome::_switchCurrentExperiment);

    right->addWidget(_open_experiments_view);
    _open_experiments_view->resizeColumnsToContents();

    QHBoxLayout* right_bot = new QHBoxLayout();

    QString path{":images/icons/"};
    QString light{"lighttheme/"};
    QString dark{"darktheme/"};

    if (gGui->isDark()) // looks like we have a dark theme
        path = path + dark;
    else
        path = path + light;

    _save_current = new QPushButton();
    _save_current->setIcon(QIcon(path + "save.svg"));
    _save_current->setText("Save current experiment");
    _save_current->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _save_current->setToolTip("Save current experiment to .nsx (HDF5) file");
    connect(_save_current, &QPushButton::clicked, this, &SubframeHome::saveCurrent);

    _save_all = new QPushButton();
    _save_all->setIcon(QIcon(path + "save.svg"));
    _save_all->setText("Save all experiments");
    _save_all->setMinimumWidth(_save_current->sizeHint().width());
    _save_all->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _save_all->setToolTip("Save all experiments to .nsx (HDF5) file");

    _remove_current = new QPushButton();
    _remove_current->setIcon(QIcon(path + "delete.svg"));
    _remove_current->setText("Remove current experiment");
    _remove_current->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _remove_current->setToolTip("Remove selected experiment from list");
    connect(_remove_current, &QPushButton::clicked, this, &SubframeHome::removeCurrent);


    right_bot->addWidget(_save_current);
    right_bot->addWidget(_save_all);
    right_bot->addWidget(_remove_current);

    right->addLayout(right_bot);

    main_layout->addLayout(right);

    _open_experiments_view->resizeColumnsToContents();
}

void SubframeHome::createNew()
{
    std::unique_ptr<ExperimentDialog> exp_dialog(//new ExperimentDialog());
        new ExperimentDialog(
        QString::fromStdString(
        gSession->generateExperimentName())
        ));
    exp_dialog->exec();  

    gGui->setReady(false);
    if (exp_dialog->result()) {
        QString expr_nm = exp_dialog->experimentName();
        QString instr_nm = exp_dialog->instrumentName();

        std::unique_ptr<Project> project_ptr{gSession->createProject(expr_nm, instr_nm)};
        if (project_ptr == nullptr) {
            return;
        }
        const bool success = gSession->addProject(std::move(project_ptr));

        if (success) {
            _open_experiments_model.reset();
            _open_experiments_model = std::make_unique<ExperimentModel>();
            _open_experiments_view->setModel(_open_experiments_model.get());
            toggleUnsafeWidgets();
        }
    }

    gSession->selectProject(gSession->numExperiments()-1);
   
    QModelIndex idx = _open_experiments_view->model()->index(gSession->numExperiments()-1, 0);
    _open_experiments_view->setCurrentIndex(idx);
    _open_experiments_view->resizeColumnsToContents();
    gGui->refreshMenu();
    refreshTables();
    gGui->setReady(true);
}

void SubframeHome::loadFromFile()
{
    gGui->setReady(false);
    QSettings s;
    s.beginGroup("RecentDirectories");
    QString loadDirectory = s.value("experiment", QDir::homePath()).toString();

    QString file_path = QFileDialog::getOpenFileName(
        this, "Load the current experiment", loadDirectory, "OpenHKL file (*.nsx)");

    if (file_path.isEmpty())
        return;

    QFileInfo info(file_path);
    s.setValue("experiment", info.absolutePath());

    try {
        gSession->loadExperimentFromFile(file_path);

        _open_experiments_model.reset();
        _open_experiments_model = std::make_unique<ExperimentModel>();
        _open_experiments_view->setModel(_open_experiments_model.get());
        _updateLastLoadedList(
            QString::fromStdString(gSession->currentProject()->experiment()->name()), file_path);
        toggleUnsafeWidgets();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }

    refreshTables();
    gGui->setReady(true);
    gGui->refreshMenu();
}

void SubframeHome::saveCurrent(bool dialogue /* = false */)
{
    gGui->setReady(false);
    QSettings s;
    s.beginGroup("RecentDirectories");
    QString loadDirectory = s.value("experiment", QDir::homePath()).toString();

    auto* project = gSession->currentProject();
    if (!project)
        return;
    QString file_path;

    if (project->saved() && !dialogue) {
        file_path = project->currentFileName();
    } else {
        QString default_name = loadDirectory + "/" + project->currentFileName();
        file_path = QFileDialog::getSaveFileName(
            this, "Save the current experiment", default_name, "OpenHKL file (*.nsx)");

        if (file_path.isEmpty())
            return;
    }

    try {
        QFileInfo info(file_path);
        s.setValue("experiment", info.absolutePath());

        gSession->currentProject()->saveToFile(file_path);
        _updateLastLoadedList(
            QString::fromStdString(gSession->currentProject()->experiment()->name()), file_path);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
    gGui->setReady(true);
    gGui->refreshMenu();
}

void SubframeHome::saveAll() { }

void SubframeHome::removeCurrent()
{
    if (gSession->hasProject()){
        gSession->removeExperiment(gSession->currentProject()->id());
        toggleUnsafeWidgets();
        refreshTables();
    }
}

void SubframeHome::_switchCurrentExperiment(const QModelIndex& index)
{
    if (gSession->hasProject()){
        gSession->selectProject(index.row());
        refreshTables();
        emit _open_experiments_model->dataChanged(QModelIndex(), QModelIndex());
        refreshTables();
    }
}

void SubframeHome::saveSettings() const
{
    QSettings s;
    s.beginGroup("RecentFiles");
    s.setValue("last_loaded", QVariant::fromValue(_last_imports));
    gGui->refreshMenu();
}

void SubframeHome::readSettings()
{
    QSettings s;
    s.beginGroup("RecentFiles");
    _last_imports = s.value("last_loaded").value<QList<QStringList>>();
    gGui->refreshMenu();
}

void SubframeHome::_updateLastLoadedList(QString name, QString file_path)
{
    QStringList temp = {name, file_path};
    if (_last_imports.empty())
        _last_imports.prepend(temp);
    else if (_last_imports[0][0] != name && _last_imports[0][1] != file_path)
        _last_imports.prepend(temp);

    if (_last_imports.size() > 5)
        _last_imports.removeLast();

    _updateLastLoadedWidget();
    refreshTables();
    gGui->refreshMenu();
}

void SubframeHome::_updateLastLoadedWidget()
{
    QSignalBlocker blocker(_last_import_widget);
    _last_import_widget->clear();

    QString path{":images/icons/"};
    QString light{"lighttheme/"};
    QString dark{"darktheme/"};

    if (gGui->isDark()) // looks like we have a dark theme
        path = path + dark;
    else
        path = path + light;

    QList<QStringList>::iterator it;
    for (it = _last_imports.begin(); it != _last_imports.end(); ++it) {
        std::ostringstream oss;
        oss << (*it).at(0).toStdString() << " (" << (*it).at(1).toStdString() << ")";
        QString fullname = QString::fromStdString(oss.str());
        QListWidgetItem* item = new QListWidgetItem(QIcon(path + "beaker.svg"), fullname);
        item->setData(100, (*it).at(1));
        _last_import_widget->addItem(item);
    }
}

void SubframeHome::_loadSelectedItem(QListWidgetItem* item)
{
    gGui->setReady(false);
    try {
        gSession->loadExperimentFromFile(item->data(100).toString());
        _open_experiments_model.reset();
        _open_experiments_model = std::make_unique<ExperimentModel>();
        _open_experiments_view->setModel(_open_experiments_model.get());
        _updateLastLoadedList(
            QString::fromStdString(gSession->currentProject()->experiment()->name()),
            item->data(100).toString());
        _open_experiments_view->resizeColumnsToContents();
        toggleUnsafeWidgets();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
    gGui->refreshMenu();
    gGui->setReady(true);
}

void SubframeHome::toggleUnsafeWidgets()
{
    _save_all->setEnabled(gSession->hasProject());
    _save_current->setEnabled(gSession->hasProject());
    _save_current->setEnabled(gSession->hasProject());
    _remove_current->setEnabled(gSession->hasProject());
    /*if (_open_experiments_model->rowCount() == 0) {
        _save_all->setEnabled(false);
        _save_current->setEnabled(false);
        //_remove_current->setEnabled(false);
    }*/
}

void SubframeHome::refreshTables() const
{
    _dataset_table->clearContents();
    _peak_collections_table->clearContents();
    _unitcell_table->clearContents();
    _dataset_table->setRowCount(0);
    _peak_collections_table->setRowCount(0);
    _unitcell_table->setRowCount(0);

    if (!gSession->hasProject())
        return;  
    
    _open_experiments_view->clearSpans();  

    try {
        auto b2s = [](bool a) { return !a ? QString("No") : QString("Yes"); };
        auto Type2s = [](nsx::PeakCollectionType t) {
            switch (t) {
                case nsx::PeakCollectionType::FOUND: return QString("Found"); break;
                case nsx::PeakCollectionType::INDEXING: return QString("Indexing"); break;
                case nsx::PeakCollectionType::PREDICTED: return QString("Predicted"); break;
                default: return QString("UNNANMED"); break;
            }
        };

        if (!gSession->hasProject()) return;

        nsx::Experiment* expt = gSession->currentProject()->experiment();
        if (expt == nullptr) return;

        std::vector<std::string> pcs_names = expt->getCollectionNames();

        auto ucell_names = gSession->currentProject()->experiment()->getUnitCellNames();

        if (ucell_names.size() > 0){
            for (auto it = ucell_names.begin(); it != ucell_names.end(); ++it) {
                auto data = gSession->currentProject()->experiment()->getUnitCell(*it);
                short n = std::distance(ucell_names.begin(), it);

                if (n >= _unitcell_table->rowCount())
                    _unitcell_table->insertRow(_unitcell_table->rowCount());

                int col = 0;
                _unitcell_table->setItem(n, col++, new QTableWidgetItem(QString::number(data->id())));
                _unitcell_table->setItem(n, col++, new QTableWidgetItem(QString::fromStdString(*it)));
                _unitcell_table->setItem(
                    n, col++,
                    new QTableWidgetItem(QString::fromStdString(data->spaceGroup().symbol())));
                _unitcell_table->setItem(
                    n, col++, new QTableWidgetItem(QString::number(data->character().a)));
                _unitcell_table->setItem(
                    n, col++, new QTableWidgetItem(QString::number(data->character().b)));
                _unitcell_table->setItem(
                    n, col++, new QTableWidgetItem(QString::number(data->character().c)));
                _unitcell_table->setItem(
                    n, col++,
                    new QTableWidgetItem(QString::number(data->character().alpha / nsx::deg)));
                _unitcell_table->setItem(
                    n, col++, new QTableWidgetItem(QString::number(data->character().beta / nsx::deg)));
                _unitcell_table->setItem(
                    n, col++,
                    new QTableWidgetItem(QString::number(data->character().gamma / nsx::deg)));
            }
        _unitcell_table->resizeColumnsToContents();
        }
        auto datasets = gSession->currentProject()->allData();

        if (datasets.size() > 0){
            for (auto it = datasets.begin(); it != datasets.end(); ++it) {
                short n = std::distance(datasets.begin(), it);

                if (n >= _dataset_table->rowCount())
                    _dataset_table->insertRow(_dataset_table->rowCount());
            
                auto detector =  gSession->currentProject()->experiment()->getDiffractometer()->detector();

                _dataset_table->setItem(
                    n, 0, new QTableWidgetItem(QString::fromStdString(it->get()->name())));
                _dataset_table->setItem(
                    n, 1, new QTableWidgetItem(QString::fromStdString(it->get()
                    ->diffractometer()->name())));
                _dataset_table->setItem(
                    n, 2, new QTableWidgetItem(QString::number(it->get()->nFrames())));
                _dataset_table->setItem(
                    n, 3, new QTableWidgetItem(QString::number(it->get()->nCols())));
                _dataset_table->setItem(
                    n, 4, new QTableWidgetItem(QString::number(it->get()->nRows())));                 
                _dataset_table->setItem(
                    n, 5, new QTableWidgetItem(QString::number(detector->distance())));
                _dataset_table->setItem(
                    n, 6, new QTableWidgetItem(QString::number(it->get()->wavelength()))); 
            }
            _dataset_table->resizeColumnsToContents();
        }

        if (!pcs_names.empty()) {
            std::vector<std::string>::iterator it;
            nsx::PeakCollection* pc;

            for (it = pcs_names.begin(); it != pcs_names.end(); it++) {
                pc = gSession->currentProject()->experiment()->getPeakCollection(*it);

                short n = std::distance(pcs_names.begin(), it);               

                if (n >= _peak_collections_table->rowCount())
                    _peak_collections_table->insertRow(_peak_collections_table->rowCount());

                _peak_collections_table->setItem(
                    n, 0, new QTableWidgetItem(QString((*it).c_str())));
                _peak_collections_table->setItem(
                    n, 1, new QTableWidgetItem(QString::number(pc->numberOfPeaks())));
                _peak_collections_table->setItem(
                    n, 2, new QTableWidgetItem(QString::number(pc->numberOfValid())));
                _peak_collections_table->setItem(
                    n, 3, new QTableWidgetItem(QString::number(pc->numberOfInvalid())));
                _peak_collections_table->setItem(n, 4, new QTableWidgetItem(b2s(pc->isIndexed())));
                _peak_collections_table->setItem(
                    n, 5, new QTableWidgetItem(b2s(pc->isIntegrated())));
                _peak_collections_table->setItem(n, 6, new QTableWidgetItem(Type2s(pc->type())));
            }
            _peak_collections_table->resizeColumnsToContents();
        }
        gGui->refreshMenu();
    } catch (const std::out_of_range& e) {
    } catch (const std::exception& e) {
    }
}

void SubframeHome::clearTables()
{
    _unitcell_table->clearContents();
    _dataset_table->clearContents();
    _peak_collections_table->clearContents();
}  

void SubframeHome::setContextMenuDatasetTable(QPoint pos)
{
    if (!gSession->hasProject()) return;
    bool hasData = gSession->currentProject()->hasDataSet();
    bool hasSelection = _dataset_table->selectionModel()->selectedIndexes().size() > 0;

    QMenu* menu = new QMenu(_dataset_table);
    QAction* list_input_files = menu->addAction("Show input files");
    menu->popup(_dataset_table->mapToGlobal(pos));

    menu->addSeparator();
    QAction* remove_dataset = menu->addAction("Remove data set");
    menu->popup(_dataset_table->mapToGlobal(pos));
 
    list_input_files->setDisabled(!(hasData && hasSelection));
    remove_dataset->setDisabled(!(hasData && hasSelection));
        
    connect(list_input_files, &QAction::triggered, _dataset_table,
        [=](){
            if (hasData){
                auto items = _dataset_table->selectionModel()->selectedIndexes();
                if (items.size() > 0){
                    int row = items[0].row();
                    QString dataset = _dataset_table->item(row, 0)->text();
                    gGui->input_files_window->refreshAll();
                    gGui->input_files_window->setDataset(dataset);
                    gGui->input_files_window->exec();
                }
            }
        }
    );

    connect(remove_dataset, &QAction::triggered, _dataset_table,
        [=](){
            if (hasData){
                auto items = _dataset_table->selectionModel()->selectedIndexes();
                if (items.size() > 0){
                    int row = items[0].row();
                    QString dataset = _dataset_table->item(row, 0)->text();
                    QMessageBox::StandardButton reply;
                    reply = QMessageBox::question(
                        this,
                        "Removing data set",
                        "Do you want to delete dataset: " + dataset,
                        QMessageBox::Yes|QMessageBox::No
                    );

                    if (reply == QMessageBox::StandardButton::Yes){
                        gGui->setReady(false);         
                        gSession->currentProject()->experiment()->removeData(dataset.toStdString());
                        gSession->onDataChanged();                
                        refreshTables();
                        gGui->setReady(true);
                    }
                }
            }
        });
}

void SubframeHome::setContextMenuPeakTable(QPoint pos)
{
    if (!gSession->hasProject()) return;
    bool hasPeakCollection = gSession->currentProject()->hasPeakCollection();
    bool hasSelection = _peak_collections_table->selectionModel()->selectedIndexes().size() > 0;

    QMenu* menu = new QMenu(_peak_collections_table);
    QAction* show_peaklist = menu->addAction("Show found peaks");
    menu->popup(_peak_collections_table->mapToGlobal(pos));

    QAction* clone_pc = menu->addAction("Clone PeakCollection");
    QAction* remove_pc = menu->addAction("Remove PeakCollection");

    show_peaklist->setDisabled(!(hasPeakCollection && hasSelection));
    clone_pc->setDisabled(!(hasPeakCollection && hasSelection));
    remove_pc->setDisabled(!(hasPeakCollection && hasSelection));

    connect(show_peaklist, &QAction::triggered, _peak_collections_table,
        [=](){
            if (hasPeakCollection){
                auto items = _peak_collections_table->selectionModel()->selectedIndexes();
                if (items.size() > 0){
                    int row = items[0].row(); // we only care for one (first) selected item
                    QString pc_name = _peak_collections_table->item(row, 0)->text();
                    gGui->peak_list_window->refreshAll();
                    gGui->peak_list_window->setPeakCollection(pc_name);
                    gGui->peak_list_window->exec();
                }
            }
        }
    );

    connect(clone_pc, &QAction::triggered, _peak_collections_table,
        [=](){
            if (hasPeakCollection){
                auto items = _peak_collections_table->selectionModel()->selectedIndexes();
                if (items.size() > 0){
                    int row = items[0].row(); // we only care for one (first) selected item
                    QString pc_name = _peak_collections_table->item(row, 0)->text();

                    bool t = true;
                    QString txt =  "Enter name for cloned PeakCollection of " + pc_name;
                    QString cloned = QInputDialog::getText(
                        this, 
                        tr("Cloning PeakCollection"),
                        tr(txt.toStdString().c_str()), 
                        QLineEdit::Normal,
                        QString::fromStdString(gSession->currentProject()->experiment()->generatePeakCollectionName()),
                        &t);

                    if (cloned.isEmpty()) return;
                    gGui->setReady(false);
                    gSession->currentProject()->clonePeakCollection(pc_name, cloned);
                    gSession->onPeaksChanged();
                    refreshTables();
                    gGui->setReady(true);
                }
            }
        }
    );

    connect(remove_pc, &QAction::triggered, _peak_collections_table,
        [=](){
            if (hasPeakCollection){
                auto items = _peak_collections_table->selectionModel()->selectedIndexes();
                if (items.size() > 0){
                    int row = items[0].row(); // we only care for one (first) selected item
                    QString pc_name = _peak_collections_table->item(row, 0)->text();

                    QMessageBox::StandardButton reply;
                    reply = QMessageBox::question(
                        this, 
                        "Removing PeakCollection", 
                        "Do you want to delete PeakCollection: " + pc_name,
                        QMessageBox::Yes|QMessageBox::No
                    );

                    if (reply == QMessageBox::StandardButton::Yes){ 
                        gGui->setReady(false);

                        gSession->currentProject()->experiment()->removePeakCollection(pc_name.toStdString());
                        gSession->currentProject()->removePeakModel(pc_name);
                        gSession->onPeaksChanged();              

                        gGui->sideBar()->refreshCurrent();
                        refreshTables();
                        gGui->setReady(true);
                    }
                }

            }
        }
    );
}

void SubframeHome::setContextMenuUnitCellTable(QPoint pos)
{
    if (!gSession->hasProject()) return;
    bool hasUnitCell = gSession->currentProject()->hasUnitCell();
    bool hasSelection = _unitcell_table->selectionModel()->selectedIndexes().size() > 0;

    QMenu* menu = new QMenu(_unitcell_table);
    QAction* removing_unit_cell = menu->addAction("Removing Unit Cell");
    menu->popup(_unitcell_table->mapToGlobal(pos));

    removing_unit_cell->setDisabled(!(hasUnitCell && hasSelection));

    connect(removing_unit_cell, &QAction::triggered, _unitcell_table,
        [=](){
            if (hasUnitCell){
                auto items = _unitcell_table->selectionModel()->selectedIndexes();
                if (items.size() > 0){
                    int row = items[0].row();
                    QString ucell_name = _unitcell_table->item(row, 1)->text();

                    QMessageBox::StandardButton reply;
                    reply = QMessageBox::question(
                        this,
                        "Removing UnitCell", 
                        "Do you want to delete UnitCell: " + ucell_name,
                        QMessageBox::Yes|QMessageBox::No
                    );

                    if (reply == QMessageBox::StandardButton::Yes){ 
                        gGui->setReady(false); 
                        gSession->currentProject()->experiment()->removeUnitCell(ucell_name.toStdString());
                        gSession->onUnitCellChanged();            
                        gGui->sideBar()->refreshCurrent();
                        refreshTables();
                        gGui->setReady(true);
                    }
                }
            }
        }
    );
}
