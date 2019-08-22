//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeHome.h
//! @brief     Defines class SubframeHome
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_SUBFRAMEHOME_H
#define GUI_PANELS_SUBFRAMEHOME_H

#include "gui/models/ExperimentModel.h"
#include "gui/views/ExperimentTableView.h"

#include <memory>

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class SubframeHome : public QWidget {
 public:
    SubframeHome();

 private:
    void setLeftLayout(QHBoxLayout* main_layout);
    void setRightLayout(QHBoxLayout* main_layout);

private:
    void createNew();
    void loadFromFile(); 
    void switchCurrentExperiment( const QModelIndex & index ) const;

    void saveCurrent();
    void saveAll();

    void readSettings();
    void saveSettings() const;

private:
    QPushButton* new_exp;
    QPushButton* old_exp;

    QPushButton* save_current;
    QPushButton* save_all;

    ExperimentTableView* _open_experiments_view;
    std::unique_ptr<ExperimentModel> _open_experiments_model;

    QList<QString> _slast_imports;
};

#endif // GUI_PANELS_SUBFRAMEHOME_H
