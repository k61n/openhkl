//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSpacerItem>
#include <qboxlayout.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qtablewidget.h>

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
}

void SubframeHome::_setLeftLayout(QHBoxLayout* main_layout)
{
    QVBoxLayout* left = new QVBoxLayout;
    QHBoxLayout* left_top = new QHBoxLayout();
    QString tooltip;

    _new_exp = new QPushButton();
    _new_exp->setIcon(QIcon(":/images/create_new.svg"));
    _new_exp->setText("Create new experiment");
    _new_exp->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    connect(_new_exp, &QPushButton::clicked, this, &SubframeHome::createNew);

    _old_exp = new QPushButton();
    _old_exp->setIcon(QIcon(":/images/load_from_folder.svg"));
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

    QSpacerItem* spacer_bottom =
        new QSpacerItem(10, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    left->addSpacerItem(spacer_bottom);

    _dataset_table = new QTableWidget(0, 5);
    _dataset_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _dataset_table->setHorizontalHeaderLabels(QStringList{
        "Name", "Diffractometer", "Number of Frames", "Number of Columns", "Number of Rows"});
    _dataset_table->resizeColumnsToContents();

    _peak_collections_table = new QTableWidget(0, 6);
    _peak_collections_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _peak_collections_table->setHorizontalHeaderLabels(QStringList{
        "Name", "Number of Peaks", "Number of Invalid Peaks", "Is indexed", "Is integrated",
        "List Type"});
    _peak_collections_table->resizeColumnsToContents();

    _unitcell_table = new QTableWidget(0, 8);
    _unitcell_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _unitcell_table->setHorizontalHeaderLabels(QStringList{
        "Name", "Space Group", "a", "b", "c", QChar(0xb1, 0x03), QChar(0xb2, 0x03),
        QChar(0xb3, 0x03)});
    _unitcell_table->resizeColumnsToContents();

    // labels for tables
    QLabel* lab_dataset = new QLabel("Datasets", this);
    QLabel* lab_peaks = new QLabel("Peak Collections", this);
    QLabel* lab_unitcell = new QLabel("Unit Cells", this);

    QVBoxLayout* lay_datasets = new QVBoxLayout();
    QVBoxLayout* lay_peaks = new QVBoxLayout();
    QVBoxLayout* lay_unitcells = new QVBoxLayout();

    lay_datasets->addWidget(lab_dataset);
    lay_datasets->addWidget(_dataset_table);

    lay_peaks->addWidget(lab_peaks);
    lay_peaks->addWidget(_peak_collections_table);

    lay_unitcells->addWidget(lab_unitcell);
    lay_unitcells->addWidget(_unitcell_table);

    left->addLayout(lay_datasets);
    left->addLayout(lay_peaks);
    left->addLayout(lay_unitcells);

    refreshTables();

    main_layout->addLayout(left);
}

void SubframeHome::_setRightLayout(QHBoxLayout* main_layout)
{
    QVBoxLayout* right = new QVBoxLayout;
    QString tooltip;

    _open_experiments_model = std::make_unique<ExperimentModel>();
    _open_experiments_view = new ExperimentTableView();
    _open_experiments_view->setModel(_open_experiments_model.get());
    connect(
        _open_experiments_view, &ExperimentTableView::doubleClicked, this,
        &SubframeHome::_switchCurrentExperiment);
    connect(
        _open_experiments_view, &ExperimentTableView::clicked, this,
        &SubframeHome::toggleUnsafeWidgets);

    right->addWidget(_open_experiments_view);

    QHBoxLayout* right_bot = new QHBoxLayout();

    _save_current = new QPushButton();
    _save_current->setIcon(QIcon(":/images/save.svg"));
    _save_current->setText("Save current experiment");
    _save_current->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    tooltip = "Save current experiment to .nsx (HDF5) file";
    _save_current->setToolTip(tooltip);
    connect(_save_current, &QPushButton::clicked, this, &SubframeHome::saveCurrent);

    _save_all = new QPushButton();
    _save_all->setIcon(QIcon(":/images/save.svg"));
    _save_all->setText("Save all experiments");
    _save_all->setMinimumWidth(_save_current->sizeHint().width());
    _save_all->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    tooltip = "Save all experiments to .nsx (HDF5) files";
    _save_all->setToolTip(tooltip);

    right_bot->addWidget(_save_current);
    right_bot->addWidget(_save_all);

    right->addLayout(right_bot);

    main_layout->addLayout(right);

    _open_experiments_view->resizeColumnsToContents();
}

void SubframeHome::createNew()
{
    std::unique_ptr<ExperimentDialog> exp_dialog(new ExperimentDialog);
    exp_dialog->exec();

    if (exp_dialog->result()) {
        QString expr_nm = exp_dialog->experimentName();
        QString instr_nm = exp_dialog->instrumentName();
       
        std::unique_ptr<Project> project_ptr {gSession->createProject
                                              (expr_nm, instr_nm)};
        if (project_ptr == nullptr){            
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
}

void SubframeHome::loadFromFile()
{
    gGui->setReady(false);
    QSettings s;
    s.beginGroup("RecentDirectories");
    QString loadDirectory = s.value("experiment", QDir::homePath()).toString();

    QString file_path = QFileDialog::getOpenFileName(
        this, "Load the current experiment", loadDirectory, "NSXTool file (*.nsx)");

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
}

void SubframeHome::saveCurrent()
{
    gGui->setReady(false);
    QSettings s;
    s.beginGroup("RecentDirectories");
    QString loadDirectory = s.value("experiment", QDir::homePath()).toString();

    try {
        QString file_path = QFileDialog::getSaveFileName(
            this, "Save the current experiment", loadDirectory, "NSXTool file (*.nsx)");

        if (file_path.isEmpty())
            return;

        QFileInfo info(file_path);
        s.setValue("experiment", info.absolutePath());

        gSession->currentProject()->saveToFile(file_path);
        _updateLastLoadedList(
            QString::fromStdString(gSession->currentProject()->experiment()->name()), file_path);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
    gGui->setReady(true);
}

void SubframeHome::saveAll() { }

void SubframeHome::_switchCurrentExperiment(const QModelIndex& index) const
{
    gSession->selectProject(index.row());
    emit _open_experiments_model->dataChanged(QModelIndex(), QModelIndex());
}

void SubframeHome::saveSettings() const
{
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("last_loaded");
    QSettings s;
    s.beginGroup("RecentFiles");
    s.setValue("last_loaded", QVariant::fromValue(_last_imports));
}

void SubframeHome::readSettings()
{
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("last_loaded");
    QSettings s;
    s.beginGroup("RecentFiles");
    _last_imports = s.value("last_loaded").value<QList<QStringList>>();
}

void SubframeHome::_updateLastLoadedList(QString name, QString file_path)
{
    QStringList temp = {name, file_path};
    if (_last_imports.empty())
        _last_imports.prepend(temp);
    else if (_last_imports[0][0] != name)
        _last_imports.prepend(temp);

    if (_last_imports.size() > 5)
        _last_imports.removeLast();

    _updateLastLoadedWidget();
    refreshTables();
}

void SubframeHome::_updateLastLoadedWidget()
{
    _last_import_widget->blockSignals(true);
    _last_import_widget->clear();

    QList<QStringList>::iterator it;
    for (it = _last_imports.begin(); it != _last_imports.end(); ++it) {
        std::ostringstream oss;
        oss << (*it).at(0).toStdString() << " (" << (*it).at(1).toStdString() << ")";
        QString fullname = QString::fromStdString(oss.str());
        QListWidgetItem* item = new QListWidgetItem(QIcon(":/images/experimentIcon.png"), fullname);
        item->setData(100, (*it).at(1));
        _last_import_widget->addItem(item);
    }
    _last_import_widget->blockSignals(false);
}

void SubframeHome::_loadSelectedItem(QListWidgetItem* item)
{
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
}

void SubframeHome::toggleUnsafeWidgets()
{
    _save_all->setEnabled(true);
    _save_current->setEnabled(true);
    _save_current->setEnabled(true);
    if (_open_experiments_model->rowCount() == 0) {
        _save_all->setEnabled(false);
        _save_current->setEnabled(false);       
    }
}

void SubframeHome::refreshTables()
{
    try {
        auto b2s = [](bool a) { return !a ? QString("No") : QString("Yes"); };
        auto Type2s = [](nsx::listtype t) {
            switch (t) {
                case nsx::listtype::FILTERED: return QString("Filtered"); break;
                case nsx::listtype::FOUND: return QString("Found"); break;
                case nsx::listtype::INDEXING: return QString("Indexing"); break;
                case nsx::listtype::PREDICTED: return QString("Predicted"); break;
                default: return QString("UNNANMED"); break;
            }
        };

        std::vector<std::string> pcs_names =
            gSession->currentProject()->experiment()->getCollectionNames();

        auto ucell_names = gSession->currentProject()->experiment()->getUnitCellNames();

        for (auto it = ucell_names.begin(); it != ucell_names.end(); ++it) {

            auto data = gSession->currentProject()->experiment()->getUnitCell(*it);
            short n = std::distance(ucell_names.begin(), it);

            if (n >= _unitcell_table->rowCount())
                _unitcell_table->insertRow(_unitcell_table->rowCount());

            _unitcell_table->setItem(n, 0, new QTableWidgetItem(QString::fromStdString(*it)));
            _unitcell_table->setItem(
                n, 1, new QTableWidgetItem(QString::fromStdString(data->spaceGroup().symbol())));
            _unitcell_table->setItem(
                n, 2, new QTableWidgetItem(QString::number(data->character().a)));
            _unitcell_table->setItem(
                n, 3, new QTableWidgetItem(QString::number(data->character().b)));
            _unitcell_table->setItem(
                n, 4, new QTableWidgetItem(QString::number(data->character().c)));
            _unitcell_table->setItem(
                n, 5, new QTableWidgetItem(QString::number(data->character().alpha / nsx::deg)));
            _unitcell_table->setItem(
                n, 6, new QTableWidgetItem(QString::number(data->character().beta / nsx::deg)));
            _unitcell_table->setItem(
                n, 7, new QTableWidgetItem(QString::number(data->character().gamma / nsx::deg)));
        }
        _unitcell_table->resizeColumnsToContents();

        auto datasets = gSession->currentProject()->allData();

        for (auto it = datasets.begin(); it != datasets.end(); ++it) {
            short n = std::distance(datasets.begin(), it);

            if (n >= _dataset_table->rowCount())
                _dataset_table->insertRow(_dataset_table->rowCount());

            _dataset_table->setItem(
                n, 0, new QTableWidgetItem(QString::fromStdString(it->get()->name())));
            _dataset_table->setItem(
                n, 1,
                new QTableWidgetItem(QString::fromStdString(it->get()->diffractometer()->name())));
            _dataset_table->setItem(
                n, 2, new QTableWidgetItem(QString::number(it->get()->nFrames())));
            _dataset_table->setItem(
                n, 3, new QTableWidgetItem(QString::number(it->get()->nCols())));
            _dataset_table->setItem(
                n, 4, new QTableWidgetItem(QString::number(it->get()->nRows())));
        }
        _dataset_table->resizeColumnsToContents();

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
                    n, 2, new QTableWidgetItem(QString::number(pc->numberOfInvalid())));
                _peak_collections_table->setItem(n, 3, new QTableWidgetItem(b2s(pc->isIndexed())));
                _peak_collections_table->setItem(
                    n, 4, new QTableWidgetItem(b2s(pc->isIntegrated())));
                _peak_collections_table->setItem(n, 5, new QTableWidgetItem(Type2s(pc->type())));
            }
            _peak_collections_table->resizeColumnsToContents();
        }
    } catch (const std::out_of_range& e) {
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
}
