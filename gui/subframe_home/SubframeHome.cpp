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
#include "core/experiment/Experiment.h"
#include "gui/dialogs/ExperimentDialog.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include <QFileDialog>
#include <QSettings>
#include <QSpacerItem>

SubframeHome::SubframeHome() : QWidget()
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
    _updateLastLoadedWidget();
}

void SubframeHome::_setLeftLayout(QHBoxLayout* main_layout)
{
    QVBoxLayout* left = new QVBoxLayout;
    QHBoxLayout* left_top = new QHBoxLayout();

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
        new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    left->addSpacerItem(spacer_bottom);

    main_layout->addLayout(left);
}

void SubframeHome::_setRightLayout(QHBoxLayout* main_layout)
{
    QVBoxLayout* right = new QVBoxLayout;

    _open_experiments_model = std::make_unique<ExperimentModel>();
    _open_experiments_view = new ExperimentTableView();
    _open_experiments_view->setModel(_open_experiments_model.get());
    connect(
        _open_experiments_view, &ExperimentTableView::doubleClicked, this,
        &SubframeHome::_switchCurrentExperiment);

    right->addWidget(_open_experiments_view);

    QHBoxLayout* right_bot = new QHBoxLayout();

    _save_current = new QPushButton();
    _save_current->setIcon(QIcon(":/images/save.svg"));
    _save_current->setText("Save current");
    _save_current->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    connect(_save_current, &QPushButton::clicked, this, &SubframeHome::saveCurrent);

    _save_all = new QPushButton();
    _save_all->setIcon(QIcon(":/images/save.svg"));
    _save_all->setText("Save all");
    _save_all->setMinimumWidth(_save_current->sizeHint().width());
    _save_all->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    right_bot->addWidget(_save_current);
    right_bot->addWidget(_save_all);

    right->addLayout(right_bot);

    main_layout->addLayout(right);
}

void SubframeHome::createNew()
{
    std::unique_ptr<ExperimentDialog> exp_dialog(new ExperimentDialog);
    exp_dialog->exec();

    if (exp_dialog->result()) {
        QString name = exp_dialog->experimentName();
        QString type = exp_dialog->instrumentName();
        bool success = gSession->createExperiment(name, type);

        if (success) {
            _open_experiments_model.reset();
            _open_experiments_model = std::make_unique<ExperimentModel>();
            _open_experiments_view->setModel(_open_experiments_model.get());
        }
    }
}

void SubframeHome::loadFromFile()
{
    QString file_path = QFileDialog::getOpenFileName(
        this, "Load the current experiment", "", "NSXTool file (*.nsx)");

    if (file_path.isEmpty())
        return;

    try {
        gSession->loadExperimentFromFile(file_path);

        _open_experiments_model.reset();
        _open_experiments_model = std::make_unique<ExperimentModel>();
        _open_experiments_view->setModel(_open_experiments_model.get());
        _updateLastLoadedList(
            QString::fromStdString(gSession->currentProject()->experiment()->name()), file_path);
    } catch (...) {
        ; // TODO: handle exception
    }
}

void SubframeHome::saveCurrent()
{
    try {
        QString file_path = QFileDialog::getSaveFileName(
            this, "Save the current experiment", "", "NSXTool file (*.nsx)");
        gSession->currentProject()->saveToFile(file_path);
        _updateLastLoadedList(
            QString::fromStdString(gSession->currentProject()->experiment()->name()), file_path);
    } catch (...) {
        ; // TODO: handle exception
    }
}

void SubframeHome::saveAll() { }

void SubframeHome::_switchCurrentExperiment(const QModelIndex& index) const
{
    gSession->selectExperiment(index.row());
    emit _open_experiments_model->dataChanged(QModelIndex(), QModelIndex());
}

void SubframeHome::saveSettings() const
{
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("last_loaded.ini");
    QSettings setting("last_loaded.ini", QSettings::IniFormat);
    setting.setValue("lastLoaded", QVariant::fromValue(_last_imports));
}

void SubframeHome::readSettings()
{
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("last_loaded.ini");
    QSettings setting("last_loaded.ini", QSettings::IniFormat);
    _last_imports = setting.value("lastLoaded").value<QList<QStringList>>();
}

void SubframeHome::_updateLastLoadedList(QString name, QString file_path)
{
    QStringList temp = {name, file_path};
    if (_last_imports.size() == 0)
        _last_imports.prepend(temp);
    else if (!(_last_imports[0][0] == name))
        _last_imports.prepend(temp);
    if (_last_imports.size() > 5)
        _last_imports.removeLast();

    _updateLastLoadedWidget();
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
    } catch (...) {
        ; // TODO: handle exception
    }
}
