//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/InputFilesWindows.cpp
//! @brief     Implements class InputFilesWindows
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subwindows/InputFilesWindow.h"

#include "base/utils/StringIO.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/experiment/Experiment.h"
#include "gui/MainWin.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTableWidget>

#include <regex>
#include <vector>

InputFilesWindow::InputFilesWindow(QWidget* parent) : QDialog(parent)
{
    setModal(false);

    auto layout = new QVBoxLayout(this);
    auto header_layout = new QHBoxLayout();

    setWindowTitle("Input Files Window");

    QLabel* label = new QLabel("Input files for dataset:");
    _data_set = new QComboBox();
    _data_set->setMaximumSize(QSize(300, 50));
    header_layout->addWidget(label);
    header_layout->addWidget(_data_set);


    _files_table = new QTableWidget();
    _files_table->setColumnCount(2);
    _files_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _files_table->setHorizontalHeaderLabels(QStringList{"ID", "Path"});
    _files_table->resizeColumnsToContents();
    _files_table->verticalHeader()->setVisible(false);

    layout->addLayout(header_layout);
    layout->addWidget(_files_table);

    resize(800, 600);
    connect(
        _data_set, qOverload<int>(&QComboBox::currentIndexChanged), this,
        &InputFilesWindow::on_combobox_select);
}

void InputFilesWindow::setDataset(QString set)
{
    int index = _data_set->findText(set);
    _data_set->setItemText(index, set);
}

void InputFilesWindow::refreshAll()
{
    if (gSession->hasProject()) {
        Project* prj = gSession->currentProject();
        auto allData = prj->allData();
        auto nData = prj->allData().size();
        int id;

        if (_data_set->count()
            > 0) { // clear combobox -- needed both clear() and removeItem to be stable working
            _data_set->clear();
            for (int i = 0; i <= _data_set->count(); i++)
                _data_set->removeItem(i);
            _data_set->setCurrentIndex(-1);
        }
        if (nData > 0) {
            for (int i = 0; i < nData; i++) {
                _data_set->addItem(QString::fromStdString(prj->getData(i)->name()));
            }
            id = _data_set->currentIndex();
            if ((id == -1) || (id >= nData))
                id = 0; // selects dataset by selected row in table

            ohkl::sptrDataSet data = prj->getData(id);

            const ohkl::MetaData& metadata = data->metadata();
            const ohkl::MetaDataMap& map = metadata.map();

            _files_table->clear();
            _files_table->setRowCount(0);

            for (auto element : map) {
                if (element.first == "sources") {
                    auto input = std::get<std::string>(element.second);

                    std::regex re("[\\|,:]");
                    // the '-1' is what makes the regex split (-1 := what was not matched)
                    std::sregex_token_iterator first{input.begin(), input.end(), re, -1}, last;
                    std::vector<std::string> tokens{first, last};

                    for (auto& e : tokens) {
                        _files_table->insertRow(_files_table->rowCount());
                        _files_table->setItem(
                            _files_table->rowCount() - 1, 0,
                            new QTableWidgetItem(QString::number(_files_table->rowCount() - 1)));
                        _files_table->setItem(
                            _files_table->rowCount() - 1, 1,
                            new QTableWidgetItem(QString::fromStdString(e)));
                    }
                }
            }
        }
    }
    _files_table->resizeColumnsToContents();
}

void InputFilesWindow::on_combobox_select()
{
    if (gSession->hasProject()) {
        Project* prj = gSession->currentProject();
        auto allData = prj->allData();
        auto nData = prj->allData().size();
        int id = _data_set->currentIndex();

        if (nData > 0 && id >= 0 && id < nData) {
            ohkl::sptrDataSet data = prj->getData(id);
            const ohkl::MetaData& metadata = data->metadata();
            const ohkl::MetaDataMap& map = metadata.map();

            _files_table->clear();
            _files_table->setRowCount(0);
            _files_table->setHorizontalHeaderLabels(QStringList{"ID", "Path"});

            for (auto element : map) {
                if (element.first == "sources") {
                    auto input = std::get<std::string>(element.second);

                    std::regex re("[\\|,:]");
                    // the '-1' is what makes the regex split (-1 := what was not matched)
                    std::sregex_token_iterator first{input.begin(), input.end(), re, -1}, last;
                    std::vector<std::string> tokens{first, last};

                    for (auto& e : tokens) {
                        _files_table->insertRow(_files_table->rowCount());
                        _files_table->setItem(
                            _files_table->rowCount() - 1, 0,
                            new QTableWidgetItem(QString::number(_files_table->rowCount() - 1)));
                        _files_table->setItem(
                            _files_table->rowCount() - 1, 1,
                            new QTableWidgetItem(QString::fromStdString(e)));
                    }
                }
            }
        }
    }
    _files_table->resizeColumnsToContents();
}
