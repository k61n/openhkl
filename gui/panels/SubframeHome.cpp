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
#include <QCR/widgets/actions.h>
#include <QCR/widgets/tables.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

class ExperimentTableModel : public QcrTableModel {
 public:
    ExperimentTableModel();

    void onClicked(const QModelIndex& cell) override;
    int columnCount() const final;
    int rowCount() const final;
    int highlighted() const final;
    void onHighlight(int i) final;
    QVariant data(const QModelIndex&, int) const final;
};

ExperimentTableModel::ExperimentTableModel()
    : QcrTableModel{"homeExperimentsTable"}
{
}

void ExperimentTableModel::onClicked(const QModelIndex &cell)
{
    gSession->selectExperiment(cell.row());
}

int ExperimentTableModel::columnCount() const
{
    return 1;
}

int ExperimentTableModel::rowCount() const
{
    return gSession->numExperiments();
}

int ExperimentTableModel::highlighted() const
{
    return gSession->selectedExperimentNum();
}

void ExperimentTableModel::onHighlight(int i)
{
    gSession->selectExperiment(i);
}

QVariant ExperimentTableModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (row < 0 || rowCount() <= row)
        return {};
    switch (role) {
        case Qt::DisplayRole: {
            return QString::fromStdString(gSession->experimentAt(row)->experiment()->name());
        }
        case Qt::ForegroundRole: {
            return QColor(Qt::black);
        }
        case Qt::BackgroundRole: {
            if (row == highlighted())
                return QColor(Qt::green);
            return QColor(Qt::white);
        }
        case Qt::ToolTipRole: {
            QString tooltip = QString::fromStdString(gSession->experimentAt(row)->experiment()->name());
            tooltip += " with data: " + gSession->experimentAt(row)->getDataNames().first();
            return tooltip;
        }
        default: return {};
    }
}

// ------------------------------------------------------------------------------------------------

class ExperimentTableView : public QcrTableView {
 public:
    ExperimentTableView();

    void onData() final;
};

ExperimentTableView::ExperimentTableView()
    : QcrTableView{new ExperimentTableModel}
{
    setSelectionMode(QAbstractItemView::NoSelection);
    onData();
}

void ExperimentTableView::onData()
{
    setHeaderHidden(false);
    setColumnWidth(0, 0);
    setColumnWidth(1, 3 * dWidth());
    for (int i = 2; i < model_->columnCount(); ++i)
        setColumnWidth(i, 7. * dWidth());
    model_->refreshModel();
    emit model_->layoutChanged();
    updateScroll();
}

// ------------------------------------------------------------------------------------------------

SubframeHome::SubframeHome()
    : QcrWidget{"HomeScreen"}
{
    QHBoxLayout* horiz = new QHBoxLayout(this);

    QVBoxLayout* left = new QVBoxLayout;
    left->addWidget(new QLabel("Open new experiment"));
    expName = new QcrLineEdit("homeExperimentName", "default");
    left->addWidget(expName);
    QHBoxLayout* line = new QHBoxLayout;
    QcrTextTriggerButton* createLoad =
            new QcrTextTriggerButton("createAndLoadButton", "Create and load data");
    createLoad->trigger()->setTriggerHook([=]() {
        gSession->createExperiment(expName->text());
        gSession->loadData();
    });
    createLoad->setToolTip("Create new Experiment and load hdf5 data");
    QcrTextTriggerButton* createImport =
            new QcrTextTriggerButton("createAndImportButton", "Create and import raw data");
    createImport->trigger()->setTriggerHook([=]() {
        gSession->createExperiment(expName->text());
        gSession->loadRawData();
    });
    createImport->setToolTip("Create new Experiment and import raw data");
    line->addWidget(createLoad);
    line->addWidget(createImport);
    left->addLayout(line);
    left->addItem(new QSpacerItem(30, 30, QSizePolicy::Expanding, QSizePolicy::Expanding));
    horiz->addLayout(left);

    QVBoxLayout* right = new QVBoxLayout;
    right->addWidget(new QLabel("Open experiments"));
    ExperimentTableView* view = new ExperimentTableView;
    right->addWidget(view);
    horiz->addLayout(right);

    setRemake([=]() {
        view->onData();
        qDebug() << "home on data";
    });
}
