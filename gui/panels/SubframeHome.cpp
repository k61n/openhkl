//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeHome.cpp
//! @brief     Implements class SubframeHome
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/panels/SubframeHome.h"

#include "gui/models/Session.h"
#include "gui/dialogs/ExperimentDialog.h"

#include <QSpacerItem>
#include <QSettings>
#include <QFileDialog>


SubframeHome::SubframeHome()
    : QWidget()
{
    QVBoxLayout* main_layout = new QVBoxLayout(this);

    QSpacerItem* spacer_top = new QSpacerItem(
        10, 50, QSizePolicy::Minimum, QSizePolicy::Fixed);
    main_layout->addSpacerItem(spacer_top);
    
    QHBoxLayout* sub_layout = new QHBoxLayout();
    QSpacerItem* spacer_left = new QSpacerItem(
        50, 10, QSizePolicy::Fixed, QSizePolicy::Minimum);
    sub_layout->addSpacerItem(spacer_left);

    setLeftLayout(sub_layout);
    setRightLayout(sub_layout);

    QSpacerItem* spacer_right = new QSpacerItem(
        10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    sub_layout->addSpacerItem(spacer_right);
    main_layout->addLayout(sub_layout);

    QSpacerItem* spacer_bot = new QSpacerItem(
        10, 50, QSizePolicy::Minimum, QSizePolicy::Fixed);
    main_layout->addSpacerItem(spacer_bot);

}

void SubframeHome::setLeftLayout(QHBoxLayout* main_layout)
{
    QVBoxLayout* left = new QVBoxLayout;

    QHBoxLayout* left_top = new QHBoxLayout(); 

    new_exp = new QPushButton();
    new_exp->setIcon(QIcon(":/images/create_new.svg"));
    new_exp->setText("Create new experiment");
    new_exp->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    connect(
        new_exp, &QPushButton::clicked,
        this, &SubframeHome::createNew
    );

    old_exp = new QPushButton();
    old_exp->setIcon(QIcon(":/images/load_from_folder.svg"));
    old_exp->setText("Load from file");
    old_exp->setMinimumWidth(new_exp->sizeHint().width());
    old_exp->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    left_top->addWidget(new_exp);
    left_top->addWidget(old_exp);

    left->addLayout(left_top);

    QSpacerItem* spacer_bottom = new QSpacerItem(
        10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    left->addSpacerItem(spacer_bottom);

    main_layout->addLayout(left);
}

void SubframeHome::setRightLayout(QHBoxLayout* main_layout)
{
    QVBoxLayout* right = new QVBoxLayout;
    
    _open_experiments_model = std::make_unique<ExperimentModel>();
    _open_experiments_view = new ExperimentTableView();
    _open_experiments_view->setModel(_open_experiments_model.get());
    connect(
        _open_experiments_view, &ExperimentTableView::doubleClicked,
        this, &SubframeHome::switchCurrentExperiment
    );

    right->addWidget(_open_experiments_view);

    QHBoxLayout* right_bot = new QHBoxLayout(); 

    save_current = new QPushButton();
    save_current->setIcon(QIcon(":/images/save.svg"));
    save_current->setText("Save current");
    save_current->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    connect(
        save_current, &QPushButton::clicked,
        this, &SubframeHome::saveCurrent
    );

    save_all = new QPushButton();
    save_all->setIcon(QIcon(":/images/save.svg"));
    save_all->setText("Save all");
    save_all->setMinimumWidth(save_current->sizeHint().width());
    save_all->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    right_bot->addWidget(save_current);
    right_bot->addWidget(save_all);

    right->addLayout(right_bot);

    main_layout->addLayout(right);
}

void SubframeHome::createNew()
{
    std::unique_ptr<ExperimentDialog> exp_dialog(new ExperimentDialog);
    exp_dialog->exec();
    
    if (exp_dialog->result()){
        QString name = exp_dialog->experimentName();
        QString type = exp_dialog->instrumentName();
        gSession->createExperiment(name, type);

        _open_experiments_model.reset();
        _open_experiments_model = std::make_unique<ExperimentModel>();
        _open_experiments_view->setModel(_open_experiments_model.get());
    }
}

void SubframeHome::loadFromFile()
{
    gSession->loadData();
}

void SubframeHome::saveCurrent()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save the current experiment"), "",
        tr("Address Book (*.nsx);;All Files (*)"));
    bool success = gSession->selectedExperiment()->saveToFile(fileName);
}

void SubframeHome::saveAll()
{

}

void SubframeHome::switchCurrentExperiment( const QModelIndex & index ) const
{
    gSession->selectExperiment(index.row());
    emit _open_experiments_model->dataChanged(QModelIndex(), QModelIndex());
}

void SubframeHome::saveSettings() const
{
    QSettings s;
    s.beginGroup("lastLoaded");
    // s.setValue("geometry", saveGeometry());
    // s.setValue("state", saveState());
}

void SubframeHome::readSettings()
{
    QSettings s;
    s.beginGroup("lastLoaded");
    // restoreGeometry(s.value("geometry").toByteArray());
    // restoreState(s.value("state").toByteArray());
}
