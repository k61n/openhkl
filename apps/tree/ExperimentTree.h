//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/ExperimentTree.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <QPoint>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>

#include "core/data/DataTypes.h"

class ExperimentItem;
class MainWindow;
class SessionModel;

class ExperimentTree : public QTreeView {
    Q_OBJECT
public:
    explicit ExperimentTree(QWidget* parent);

    ~ExperimentTree();

    SessionModel* session();

signals:
    void plotData(nsx::sptrDataSet);
    void inspectWidget(QWidget*);
    void resetScene();

public slots:
    void keyPressEvent(QKeyEvent* event);
    void onCustomMenuRequested(const QPoint& point);
    void onDoubleClick(const QModelIndex& index);

    void onSingleClick(const QModelIndex& index);

private:
    void openInstrumentStatesDialog();
};
