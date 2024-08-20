//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_home/SubframeHome.cpp
//! @brief     Implements class SubframeHome
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_home/SubframeHome.h"

#include "base/utils/Units.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/loader/IDataReader.h"
#include "core/shape/PeakCollection.h"
#include "gui/MainWin.h"
#include "gui/dialogs/ExperimentDialog.h"
#include "gui/models/ExperimentModel.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/subwindows/InputFilesWindow.h"
#include "gui/subwindows/PeaklistWindow.h"
#include "gui/utility/SideBar.h"
#include "gui/views/ExperimentTableView.h"
#include "tables/crystal/UnitCell.h"

#include <QAbstractItemModel>
#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QListView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QSpacerItem>
#include <QTableWidget>

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
    QHBoxLayout* sub_layout = new QHBoxLayout();

    setLeftLayout(sub_layout);
    setRightLayout(sub_layout);
    main_layout->addLayout(sub_layout);

    readSettings();
    toggleUnsafeWidgets();
    updateLastLoadedWidget();
    gGui->refreshMenu();
}

void SubframeHome::setLeftLayout(QHBoxLayout* main_layout)
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

    _new_strategy = new QPushButton();
    _new_strategy->setIcon(QIcon(path + "plus.svg"));
    _new_strategy->setText("New experiment (strategy mode)");
    _new_strategy->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    connect(_new_strategy, &QPushButton::clicked, this, [=]() { createNew(true); });

    _old_exp = new QPushButton();
    _old_exp->setIcon(QIcon(path + "open.svg"));
    _old_exp->setText("Load experiment from file");
    _old_exp->setMinimumWidth(_new_exp->sizeHint().width());
    _old_exp->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    tooltip = "Load an existing experiment from a .ohkl (HDF5) file";
    _old_exp->setToolTip(tooltip);
    connect(_old_exp, &QPushButton::clicked, this, &SubframeHome::loadFromFile);

    left_top->addWidget(_new_exp);
    left_top->addWidget(_new_strategy);
    left_top->addWidget(_old_exp);

    left->addLayout(left_top);

    _last_import_widget = new QListWidget(this);

    _last_import_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _last_import_widget->setStyleSheet("background-color: transparent;");
    connect(_last_import_widget, &QListWidget::itemClicked, this, &SubframeHome::loadSelectedItem);

    left->addWidget(_last_import_widget);

    _open_experiments_model = std::make_unique<ExperimentModel>();
    _open_experiments_view = new ExperimentTableView();
    _open_experiments_view->setModel(_open_experiments_model.get());
    connect(
        _open_experiments_view, &ExperimentTableView::clicked, this,
        &SubframeHome::switchCurrentExperiment);

    left->addWidget(_open_experiments_view);

    QHBoxLayout* left_bot = new QHBoxLayout();

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


    left_bot->addWidget(_save_current);
    left_bot->addWidget(_save_all);
    left_bot->addWidget(_remove_current);

    left->addLayout(left_bot);

    main_layout->addLayout(left);

    _open_experiments_view->resizeColumnsToContents();
}

void SubframeHome::setRightLayout(QHBoxLayout* main_layout)
{
    QString path{":images/icons/"};
    QString light{"lighttheme/"};
    QString dark{"darktheme/"};

    if (gGui->isDark()) // looks like we have a dark theme
        path = path + dark;
    else
        path = path + light;

    QVBoxLayout* right = new QVBoxLayout();

    _add_data = new QComboBox;
    _add_single_image = new QComboBox;

    _add_data->addItem(QIcon(path + "plus.svg"), "Add data set");
    _add_data->addItem("Tiff (.tiff)");
    _add_data->addItem("Raw (.raw)");
    _add_data->addItem("Plain text");
    _add_data->addItem("Nexus (.nxs)");
    _add_data->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    _add_single_image->addItem(QIcon(path + "plus.svg"), "Add single image");
    _add_single_image->addItem("Tiff (.tiff)");
    _add_single_image->addItem("Raw (.raw)");
    _add_single_image->addItem("Plain text");
    _add_single_image->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    _dataset_table = new QTableWidget(0, 6);
    _dataset_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _dataset_table->setHorizontalHeaderLabels(QStringList{
        "Name",
        "Diffractometer",
        "Images",
        "Columns",
        "Rows",
        "Wavelength (" + QString(QChar(8491)) + ")",
    });
    _dataset_table->resizeColumnsToContents();
    _dataset_table->verticalHeader()->setVisible(false);
    _dataset_table->setContextMenuPolicy(Qt::CustomContextMenu);

    _peak_collections_table = new QTableWidget(0, 8);
    _peak_collections_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _peak_collections_table->setHorizontalHeaderLabels(QStringList{
        "Name", "Data set", "Unit cell", "Peaks", "Valid peaks", "Invalid peaks", "Indexed",
        "Integrated", "Type"});

    _peak_collections_table->resizeColumnsToContents();
    _peak_collections_table->verticalHeader()->setVisible(false);
    _peak_collections_table->setContextMenuPolicy(Qt::CustomContextMenu);

    _unitcell_table = new QTableWidget(0, 10);
    _unitcell_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _unitcell_table->setContextMenuPolicy(Qt::CustomContextMenu);
    _unitcell_table->setHorizontalHeaderLabels(QStringList{
        "ID", "Name", "Data set", "Space Group", "a", "b", "c", QChar(0xb1, 0x03),
        QChar(0xb2, 0x03), QChar(0xb3, 0x03)});
    _unitcell_table->resizeColumnsToContents();
    _unitcell_table->verticalHeader()->setVisible(false);

    // labels for tables
    QLabel* lab_dataset = new QLabel("Data sets in current experiment", this);
    QLabel* lab_peaks = new QLabel("Peak collections in current experiment", this);
    QLabel* lab_unitcell = new QLabel("Unit cells in current experiment", this);

    QGridLayout* lay_add_data = new QGridLayout();
    QHBoxLayout* lay_datasets_head = new QHBoxLayout();
    QVBoxLayout* lay_datasets = new QVBoxLayout();
    QHBoxLayout* lay_peaks_head = new QHBoxLayout();
    QVBoxLayout* lay_peaks = new QVBoxLayout();
    QVBoxLayout* lay_unitcells = new QVBoxLayout();

    int ncols = 5;
    for (int i = 0; i < ncols; ++i)
        lay_add_data->setColumnStretch(i, 1);
    lay_add_data->addWidget(_add_data, 0, ncols - 2, 1, 1);
    lay_add_data->addWidget(_add_single_image, 0, ncols - 1, 1, 1);

    lay_datasets_head->addWidget(lab_dataset);

    lay_datasets->addLayout(lay_datasets_head);
    lay_datasets->addWidget(_dataset_table);

    lay_peaks_head->addWidget(lab_peaks);
    lay_peaks->addLayout(lay_peaks_head);
    lay_peaks->addWidget(_peak_collections_table);

    lay_unitcells->addWidget(lab_unitcell);
    lay_unitcells->addWidget(_unitcell_table);

    right->addLayout(lay_add_data);
    right->addLayout(lay_datasets);
    right->addLayout(lay_peaks);
    right->addLayout(lay_unitcells);

    refreshTables();

    main_layout->addLayout(right);

    connect(_add_data, QOverload<int>::of(&QComboBox::activated), this, &SubframeHome::addDataSet);
    connect(
        _add_single_image, QOverload<int>::of(&QComboBox::activated), this,
        &SubframeHome::addSingleImage);
    connect(
        _dataset_table, &QWidget::customContextMenuRequested, this,
        &SubframeHome::setContextMenuDatasetTable);
    connect(
        _peak_collections_table, &QWidget::customContextMenuRequested, this,
        &SubframeHome::setContextMenuPeakTable);
    connect(
        _unitcell_table, &QWidget::customContextMenuRequested, this,
        &SubframeHome::setContextMenuUnitCellTable);
}

void SubframeHome::createNew(bool strategy)
{
    std::unique_ptr<ExperimentDialog> exp_dialog( // new ExperimentDialog());
        new ExperimentDialog(QString::fromStdString(gSession->generateExperimentName())));
    exp_dialog->exec();

    gGui->setReady(false);
    if (exp_dialog->result()) {
        QString expr_nm = exp_dialog->experimentName();
        QString instr_nm = exp_dialog->instrumentName();

        std::unique_ptr<Project> project_ptr{gSession->createProject(expr_nm, instr_nm, strategy)};
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

    gSession->selectProject(gSession->numExperiments() - 1);

    QModelIndex idx = _open_experiments_view->model()->index(gSession->numExperiments() - 1, 0);
    _open_experiments_view->setCurrentIndex(idx);
    _open_experiments_view->resizeColumnsToContents();
    gGui->refreshMenu();
    refreshTables();
    gGui->setReady(true);
}

void SubframeHome::loadFromFile()
{
    gGui->setReady(false);
    QSettings settings = gGui->qSettings();
    settings.beginGroup("RecentDirectories");
    QString loadDirectory = settings.value("experiment", QDir::homePath()).toString();

    QString file_path = QFileDialog::getOpenFileName(
        this, "Load the current experiment", loadDirectory, "OpenHKL file (*.ohkl)");

    if (file_path.isEmpty())
        return;

    QFileInfo info(file_path);
    settings.setValue("experiment", info.absolutePath());

    try {
        gSession->loadExperimentFromFile(file_path);

        _open_experiments_model.reset();
        _open_experiments_model = std::make_unique<ExperimentModel>();
        _open_experiments_view->setModel(_open_experiments_model.get());
        _open_experiments_view->resizeColumnsToContents();
        updateLastLoadedList(
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
    QSettings settings = gGui->qSettings();
    settings.beginGroup("RecentDirectories");
    QString loadDirectory = settings.value("experiment", QDir::homePath()).toString();

    auto* project = gSession->currentProject();
    if (!project)
        return;
    QString file_path;

    if (project->saved() && !dialogue) {
        file_path = project->currentFileName();
    } else {
        QString default_name = loadDirectory + "/" + project->currentFileName() + ".ohkl";
        file_path = QFileDialog::getSaveFileName(
            this, "Save the current experiment", default_name, "OpenHKL file (*.ohkl)");

        if (file_path.isEmpty())
            return;
    }

    try {
        QFileInfo info(file_path);
        settings.setValue("experiment", info.absolutePath());

        gSession->currentProject()->saveToFile(file_path);
        updateLastLoadedList(
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
    if (gSession->hasProject()) {
        gSession->removeExperiment(gSession->currentProject()->id());
        toggleUnsafeWidgets();
        refreshTables();
    }
}

void SubframeHome::switchCurrentExperiment(const QModelIndex& index)
{
    if (gSession->hasProject()) {
        gSession->selectProject(index.row());
        refreshTables();
        emit _open_experiments_model->dataChanged(QModelIndex(), QModelIndex());
        refreshTables();
    }
}

void SubframeHome::saveSettings() const
{
    QSettings settings = gGui->qSettings();
    settings.beginWriteArray("RecentFiles");
    for (std::size_t index = 0; index < _last_experiments.size(); ++index) {
        settings.setArrayIndex(index);
        settings.setValue("experiment_name", _last_experiments[index].first);
        settings.setValue("file_path", _last_experiments[index].second);
    }
    settings.endArray();
    gGui->refreshMenu();
}

void SubframeHome::readSettings()
{
    QSettings settings = gGui->qSettings();
    int num_recent_files = settings.beginReadArray("RecentFiles");
    for (std::size_t index = 0; index < num_recent_files; ++index) {
        settings.setArrayIndex(index);
        QString name = settings.value("experiment_name").toString();
        QString path = settings.value("file_path").toString();
        _last_experiments.append(qMakePair(name, path));
    }
    settings.endArray();
    gGui->refreshMenu();
}

void SubframeHome::updateLastLoadedList(QString name, QString file_path)
{
    if (_last_experiments.empty())
        _last_experiments.prepend(qMakePair(name, file_path));
    else if (_last_experiments[0].first != name && _last_experiments[0].second != file_path)
        _last_experiments.prepend(qMakePair(name, file_path));

    if (_last_experiments.size() > 5)
        _last_experiments.removeLast();

    updateLastLoadedWidget();
    refreshTables();
    gGui->refreshMenu();
}

void SubframeHome::updateLastLoadedWidget()
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

    if (_last_experiments.empty())
        return;
    for (std::size_t index = 0; index < _last_experiments.size(); ++index) {
        std::ostringstream oss;
        oss << _last_experiments.at(index).first.toStdString() << " ("
            << _last_experiments.at(index).second.toStdString() << ")";
        QString fullname = QString::fromStdString(oss.str());
        QListWidgetItem* item = new QListWidgetItem(QIcon(path + "beaker.svg"), fullname);
        item->setData(100, _last_experiments.at(index).second);
        _last_import_widget->addItem(item);
    }
}

void SubframeHome::loadSelectedItem(QListWidgetItem* item)
{
    gGui->setReady(false);
    try {
        gSession->loadExperimentFromFile(item->data(100).toString());
        _open_experiments_model.reset();
        _open_experiments_model = std::make_unique<ExperimentModel>();
        _open_experiments_view->setModel(_open_experiments_model.get());
        updateLastLoadedList(
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

    if (!gSession->hasProject())
        return;

    _add_data->setEnabled(!gSession->currentProject()->strategyMode());
    _add_single_image->setEnabled(gSession->currentProject()->strategyMode());
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
    _open_experiments_view->resizeColumnsToContents();

    auto b2s = [](bool a) { return !a ? QString("No") : QString("Yes"); };
    auto Type2s = [](ohkl::PeakCollectionType t) {
        switch (t) {
            case ohkl::PeakCollectionType::FOUND: return QString("Found"); break;
            case ohkl::PeakCollectionType::INDEXING: return QString("Indexing"); break;
            case ohkl::PeakCollectionType::PREDICTED: return QString("Predicted"); break;
            default: return QString("UNNANMED"); break;
        }
    };

    if (!gSession->hasProject())
        return;

    if (gSession->currentProject()->hasUnitCell()) {
        int ncell = 0;
        for (auto cell : gSession->currentProject()->experiment()->getUnitCells()) {
            if (ncell >= _unitcell_table->rowCount())
                _unitcell_table->insertRow(_unitcell_table->rowCount());

            int col = 0;
            _unitcell_table->setItem(
                ncell, col++, new QTableWidgetItem(QString::number(cell->id())));
            _unitcell_table->setItem(
                ncell, col++, new QTableWidgetItem(QString::fromStdString(cell->name())));
            _unitcell_table->setItem(
                ncell, col++, new QTableWidgetItem(QString::fromStdString(cell->data()->name())));
            _unitcell_table->setItem(
                ncell, col++,
                new QTableWidgetItem(QString::fromStdString(cell->spaceGroup().symbol())));
            _unitcell_table->setItem(
                ncell, col++, new QTableWidgetItem(QString::number(cell->character().a)));
            _unitcell_table->setItem(
                ncell, col++, new QTableWidgetItem(QString::number(cell->character().b)));
            _unitcell_table->setItem(
                ncell, col++, new QTableWidgetItem(QString::number(cell->character().c)));
            _unitcell_table->setItem(
                ncell, col++,
                new QTableWidgetItem(QString::number(cell->character().alpha / ohkl::deg)));
            _unitcell_table->setItem(
                ncell, col++,
                new QTableWidgetItem(QString::number(cell->character().beta / ohkl::deg)));
            _unitcell_table->setItem(
                ncell++, col++,
                new QTableWidgetItem(QString::number(cell->character().gamma / ohkl::deg)));
        }
        _unitcell_table->resizeColumnsToContents();
    }
    auto datasets = gSession->currentProject()->allData();

    int ndata = 0;
    if (gSession->currentProject()->hasDataSet()) {
        for (const auto& data : gSession->currentProject()->experiment()->getAllData()) {
            // for (const auto& [key, data] :
            // *(gSession->currentProject()->experiment()->getDataMap())) {

            if (ndata >= _dataset_table->rowCount())
                _dataset_table->insertRow(_dataset_table->rowCount());

            int col = 0;
            _dataset_table->setItem(
                ndata, col++, new QTableWidgetItem(QString::fromStdString(data->name())));
            _dataset_table->setItem(
                ndata, col++,
                new QTableWidgetItem(QString::fromStdString(data->diffractometer()->name())));
            _dataset_table->setItem(
                ndata, col++, new QTableWidgetItem(QString::number(data->nFrames())));
            _dataset_table->setItem(
                ndata, col++, new QTableWidgetItem(QString::number(data->nCols())));
            _dataset_table->setItem(
                ndata, col++, new QTableWidgetItem(QString::number(data->nRows())));
            _dataset_table->setItem(
                ndata++, col++, new QTableWidgetItem(QString::number(data->wavelength())));
        }
        _dataset_table->resizeColumnsToContents();
    }

    if (gSession->currentProject()->hasPeakCollection()) {

        int ncollection = 0;
        for (auto* collection : gSession->currentProject()->experiment()->getPeakCollections()) {

            std::string cell_name = "None";
            if (collection->unitCell())
                cell_name = collection->unitCell()->name();

            if (ncollection >= _peak_collections_table->rowCount())
                _peak_collections_table->insertRow(_peak_collections_table->rowCount());

            int col = 0;
            _peak_collections_table->setItem(
                ncollection, col++,
                new QTableWidgetItem(QString::fromStdString(collection->name())));
            _peak_collections_table->setItem(
                ncollection, col++,
                new QTableWidgetItem(QString::fromStdString(collection->data()->name())));
            _peak_collections_table->setItem(
                ncollection, col++, new QTableWidgetItem(QString::fromStdString(cell_name)));
            _peak_collections_table->setItem(
                ncollection, col++,
                new QTableWidgetItem(QString::number(collection->numberOfPeaks())));
            _peak_collections_table->setItem(
                ncollection, col++,
                new QTableWidgetItem(QString::number(collection->numberOfValid())));
            _peak_collections_table->setItem(
                ncollection, col++,
                new QTableWidgetItem(QString::number(collection->numberOfInvalid())));
            _peak_collections_table->setItem(
                ncollection, col++, new QTableWidgetItem(b2s(collection->isIndexed())));
            _peak_collections_table->setItem(
                ncollection, col++, new QTableWidgetItem(b2s(collection->isIntegrated())));
            _peak_collections_table->setItem(
                ncollection++, col++, new QTableWidgetItem(Type2s(collection->type())));
        }
        _peak_collections_table->resizeColumnsToContents();
    }
    gGui->refreshMenu();
}

void SubframeHome::clearTables()
{
    _unitcell_table->clearContents();
    _dataset_table->clearContents();
    _peak_collections_table->clearContents();
}

void SubframeHome::setContextMenuDatasetTable(QPoint pos)
{
    if (!gSession->hasProject())
        return;
    bool hasData = gSession->currentProject()->hasDataSet();
    bool hasSelection = _dataset_table->selectionModel()->selectedIndexes().size() > 0;

    QMenu* menu = new QMenu(_dataset_table);
    menu->popup(_dataset_table->mapToGlobal(pos));
    QAction* list_input_files = menu->addAction("Show input files");
    menu->addSeparator();
    QAction* remove_dataset = menu->addAction("Remove data set");

    list_input_files->setDisabled(!(hasData && hasSelection));
    remove_dataset->setDisabled(!(hasData && hasSelection));

    connect(list_input_files, &QAction::triggered, _dataset_table, [=]() {
        if (hasData) {
            auto items = _dataset_table->selectionModel()->selectedIndexes();
            if (items.size() > 0) {
                int row = items[0].row();
                QString dataset = _dataset_table->item(row, 0)->text();
                gGui->input_files_window->refreshAll();
                gGui->input_files_window->setDataset(dataset);
                gGui->input_files_window->exec();
            }
        }
    });

    connect(remove_dataset, &QAction::triggered, _dataset_table, [=]() {
        if (hasData) {
            auto items = _dataset_table->selectionModel()->selectedIndexes();
            if (items.size() > 0) {
                int row = items[0].row();
                QString dataset = _dataset_table->item(row, 0)->text();
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(
                    this, "Removing data set", "Do you really want to delete \"" + dataset + "\"",
                    QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::StandardButton::Yes) {
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
    if (!gSession->hasProject())
        return;
    bool hasPeakCollection = gSession->currentProject()->hasPeakCollection();
    bool hasSelection = _peak_collections_table->selectionModel()->selectedIndexes().size() > 0;

    QMenu* menu = new QMenu(_peak_collections_table);
    menu->popup(_peak_collections_table->mapToGlobal(pos));

    QAction* show_peaklist = menu->addAction("Show peak table");
    QAction* clone_pc = menu->addAction("Clone peak collection");
    QAction* remove_pc = menu->addAction("Remove peak collection");

    show_peaklist->setDisabled(!(hasPeakCollection && hasSelection));
    clone_pc->setDisabled(!(hasPeakCollection && hasSelection));
    remove_pc->setDisabled(!(hasPeakCollection && hasSelection));

    connect(show_peaklist, &QAction::triggered, _peak_collections_table, [=]() {
        if (hasPeakCollection) {
            auto items = _peak_collections_table->selectionModel()->selectedIndexes();
            if (items.size() > 0) {
                int row = items[0].row(); // we only care for one (first) selected item
                QString pc_name = _peak_collections_table->item(row, 0)->text();
                gGui->peak_list_window->refreshAll();
                gGui->peak_list_window->setPeakCollection(pc_name);
                gGui->peak_list_window->exec();
            }
        }
    });

    connect(clone_pc, &QAction::triggered, _peak_collections_table, [=]() {
        if (hasPeakCollection) {
            auto items = _peak_collections_table->selectionModel()->selectedIndexes();
            if (items.size() > 0) {
                int row = items[0].row(); // we only care for one (first) selected item
                QString pc_name = _peak_collections_table->item(row, 0)->text();

                QString txt = "Enter name for clone of \"" + pc_name + "\"";
                QString cloned = QInputDialog::getText(
                    this, tr("Cloning peak collection"), tr(txt.toStdString().c_str()),
                    QLineEdit::Normal,
                    QString::fromStdString(
                        gSession->currentProject()->experiment()->generatePeakCollectionName()));

                if (cloned.isEmpty())
                    return;
                gGui->setReady(false);
                gSession->currentProject()->clonePeakCollection(pc_name, cloned);
                gSession->onPeaksChanged();
                refreshTables();
                gGui->setReady(true);
            }
        }
    });

    connect(remove_pc, &QAction::triggered, _peak_collections_table, [=]() {
        if (hasPeakCollection) {
            auto items = _peak_collections_table->selectionModel()->selectedIndexes();
            if (items.size() > 0) {
                int row = items[0].row(); // we only care for one (first) selected item
                QString pc_name = _peak_collections_table->item(row, 0)->text();

                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(
                    this, "Removing peak collection", "Do you want really to delete " + pc_name,
                    QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::StandardButton::Yes) {
                    gGui->setReady(false);

                    gSession->currentProject()->experiment()->removePeakCollection(
                        pc_name.toStdString());
                    gSession->currentProject()->removePeakModel(pc_name);
                    gSession->onPeaksChanged();

                    gGui->sideBar()->refreshCurrent();
                    refreshTables();
                    gGui->setReady(true);
                }
            }
        }
    });
}

void SubframeHome::setContextMenuUnitCellTable(QPoint pos)
{
    if (!gSession->hasProject())
        return;
    bool hasUnitCell = gSession->currentProject()->hasUnitCell();
    bool hasSelection = _unitcell_table->selectionModel()->selectedIndexes().size() > 0;

    QMenu* menu = new QMenu(_unitcell_table);
    QAction* removing_unit_cell = menu->addAction("Remove unit cell");
    menu->popup(_unitcell_table->mapToGlobal(pos));

    removing_unit_cell->setDisabled(!(hasUnitCell && hasSelection));

    connect(removing_unit_cell, &QAction::triggered, _unitcell_table, [=]() {
        if (hasUnitCell) {
            auto items = _unitcell_table->selectionModel()->selectedIndexes();
            if (items.size() > 0) {
                int row = items[0].row();
                QString ucell_name = _unitcell_table->item(row, 1)->text();

                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(
                    this, "Removing unit cell",
                    "Do you really want to delete \"" + ucell_name + "\"",
                    QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::StandardButton::Yes) {
                    gGui->setReady(false);
                    gSession->currentProject()->experiment()->removeUnitCell(
                        ucell_name.toStdString());
                    gSession->onUnitCellChanged();
                    gGui->sideBar()->refreshCurrent();
                    refreshTables();
                    gGui->setReady(true);
                }
            }
        }
    });
}

void SubframeHome::addDataSet(int index)
{
    QSignalBlocker blocker(_add_data);
    switch (index) {
        case 1: gSession->loadImages(ohkl::DataFormat::TIFF, false); break;
        case 2: gSession->loadImages(ohkl::DataFormat::RAW, false); break;
        case 3: gSession->loadImages(ohkl::DataFormat::PLAINTEXT, false); break;
        case 4: gSession->loadData(ohkl::DataFormat::NEXUS); break;
        default: {};
    }
    gGui->sideBar()->refreshCurrent();
    _add_data->setCurrentIndex(0);
}

void SubframeHome::addSingleImage(int index)
{
    QSignalBlocker blocker(_add_single_image);
    switch (index) {
        case 1: gSession->loadImages(ohkl::DataFormat::TIFF, true); break;
        case 2: gSession->loadImages(ohkl::DataFormat::RAW, true); break;
        case 3: gSession->loadImages(ohkl::DataFormat::PLAINTEXT, true); break;
        default: {};
    }
    gGui->sideBar()->refreshCurrent();
    _add_data->setCurrentIndex(0);
}
