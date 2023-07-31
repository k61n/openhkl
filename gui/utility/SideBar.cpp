//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/SideBar.cpp
//! @brief     Implements class SideBar
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/SideBar.h"
#include "core/experiment/Experiment.h"
#include "gui/MainWin.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/subframe_experiment/SubframeExperiment.h"
#include "gui/subframe_filter/SubframeFilter.h"
#include "gui/subframe_find/SubframeFindPeaks.h"
#include "gui/subframe_home/SubframeHome.h"
#include "gui/subframe_index/SubframeAutoIndexer.h"
#include "gui/subframe_integrate/SubframeIntegrate.h"
#include "gui/subframe_merge/SubframeMerge.h"
#include "gui/subframe_predict/SubframePredict.h"
#include "gui/subframe_refiner/SubframeRefiner.h"
#include "gui/subframe_reject/SubframeReject.h"
#include "gui/subframe_shapes/SubframeShapes.h"

#include <QDebug>
#include <QEvent>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPoint>
#include <QSignalBlocker>

// TODO: find a better place for this
// Icon attributions:
// home.svg: Home by Bhuvan from the Noun Project
// experiment.svg: experiment by Nubaia Karim Barsha from the Noun Project
// finder.svg: Radar by Luiz Carvalho from the Noun Project
// filter.svg: filter by fauzin idea from the Noun Project
// indexer.svg: Crystal by Vectorstall from the Noun Project
// predictor.svg: Crystal Ball by Chanut is Industries from the Noun Project
// refiner.svg: linear regression by Becris from the Noun Project
// integrator.svg: triple integral by Sumana Chamrunworakiat from the Noun Project
// reject.svg: Reject by Alfredo @ IconsAlfredo.com
// merger.svg: Merge by Muneer A.Safiah from the Noun Project

SideBar::SideBar(QWidget* parent)
    : QWidget(parent)
    , mCheckedAction(nullptr)
    , mOverAction(nullptr)
    , _strategy(false)
{
    setMouseTracking(true);

    QString path{":images/sidebar/"};
    QString light{"lighttheme/"};
    QString dark{"darktheme/"};

    if (gGui->isDark()) // looks like we have a dark theme
        path = path + dark;
    else
        path = path + light;

    QAction* home = addAction(QIcon(path + QString("home.svg")), "Home");
    QAction* experiment = addAction(QIcon(path + QString("experiment.svg")), "Experiment");
    QAction* finder = addAction(QIcon(path + QString("finder.svg")), "Find Peaks");
    QAction* filter = addAction(QIcon(path + QString("filter.svg")), "Filter Peaks");
    QAction* indexer = addAction(QIcon(path + QString("indexer.svg")), "Index");
    QAction* shapes = addAction(QIcon(path + QString("shapes.svg")), "Shape model");
    QAction* predictor = addAction(QIcon(path + QString("predictor.svg")), "Predict");
    QAction* refiner = addAction(QIcon(path + QString("refiner.svg")), "Refine");
    QAction* integrator = addAction(QIcon(path + QString("integrator.svg")), "Integrate");
    QAction* rejector = addAction(QIcon(path + QString("reject.svg")), "Reject");
    QAction* info = addAction(QIcon(path + QString("merger.svg")), "Merge");


    QAction* tempAction = mActions.at(0);
    mCheckedAction = tempAction;
    tempAction->setChecked(true);
    update();

    connect(home, &QAction::triggered, this, &SideBar::onHome);
    connect(experiment, &QAction::triggered, this, &SideBar::onExperiment);
    connect(finder, &QAction::triggered, this, &SideBar::onFindPeaks);
    connect(filter, &QAction::triggered, this, &SideBar::onFilterPeaks);
    connect(indexer, &QAction::triggered, this, &SideBar::onIndexer);
    connect(shapes, &QAction::triggered, this, &SideBar::onShapes);
    connect(predictor, &QAction::triggered, this, &SideBar::onPredictor);
    connect(refiner, &QAction::triggered, this, &SideBar::onRefiner);
    connect(integrator, &QAction::triggered, this, &SideBar::onIntegrator);
    connect(rejector, &QAction::triggered, this, &SideBar::onReject);
    connect(info, &QAction::triggered, this, &SideBar::onMerger);
    connect(this, &SideBar::subframeChanged, this, &SideBar::onSubframeChanged);
}


void SideBar::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    QFont fontText(painter.font());
    fontText.setFamily("Helvetica Neue");
    painter.setFont(fontText);

    int action_y = 0;
    for (std::size_t idx = 0; idx < mActions.size(); ++idx) {
        auto action = mActions.at(idx);
        QRect actionRect(0, action_y, event->rect().width(), event->rect().width());
        if (_strategy && idx != 0 && idx != 1)
            painter.fillRect(actionRect, QColor(100, 100, 100));

        if (action->isChecked()) {
            QColor fill_color;
            if (gGui->isDark())
                fill_color = QColor(35, 35, 35);
            else
                fill_color = QColor(200, 200, 200);
            painter.fillRect(actionRect, fill_color);
        }

        if (action == mOverAction) {
            painter.fillRect(actionRect, QColor(150, 150, 150));
        }

        if (gGui->isDark()) // looks like we have a dark theme
            painter.setPen(Qt::white);
        else
            painter.setPen(Qt::black);

        QRect actionIconRect(2, action_y + 10, actionRect.width(), actionRect.height() - 20);
        QIcon actionIcon(action->icon());
        actionIcon.paint(&painter, actionIconRect);

        action_y += actionRect.height();
    }

    // showing names of Subframes as QToolTipText next to icons of Sidebar
    QPoint global_pos = QCursor::pos();
    QPoint mouse_pos = mapFromGlobal(global_pos);
    QPoint tips_pos; // actual qtooltip window postion - we gonna move it a bit besides the cursor

    int spacing_x = 10;
    int spacing_y = 0;

    tips_pos.setX(global_pos.x() + spacing_x);
    tips_pos.setY(global_pos.y() + spacing_y);

    int id = mouse_pos.y() / _min_icon_height; // find the mAction id
    if (_strategy && id != 0 && id != 1)
        return;

    if (id < mActions.size())
        QToolTip::showText(tips_pos, mActions.at(id)->text()); // display mActions text*/
}

QSize SideBar::minimumSizeHint() const
{
    return _min_icon_height * QSize(1, mActions.size());
}

void SideBar::addAction(QAction* action)
{
    mActions.push_back(action);
    action->setCheckable(true);
    update();
}

QAction* SideBar::addAction(const QIcon& icon, const QString& text)
{
    QAction* action = new QAction(icon, text, this);
    action->setCheckable(true);
    mActions.push_back(action);
    update();
    return action;
}

void SideBar::manualSelect(int index)
{
    QAction* tempAction = mActions[index];
    if (mCheckedAction)
        mCheckedAction->setChecked(false);
    if (mOverAction == tempAction)
        mOverAction = nullptr;
    mCheckedAction = tempAction;
    mCheckedAction->trigger();
    tempAction->setChecked(true);
    update();
}

void SideBar::mousePressEvent(QMouseEvent* event)
{
    QAction* tempAction = actionAt(event->pos());
    if (tempAction == nullptr || tempAction->isChecked())
        return;
    if (mCheckedAction)
        mCheckedAction->setChecked(false);
    if (mOverAction == tempAction)
        mOverAction = nullptr;
    mCheckedAction = tempAction;
    mCheckedAction->trigger();
    tempAction->setChecked(true);
    update();
    QWidget::mousePressEvent(event);
}

void SideBar::mouseMoveEvent(QMouseEvent* event)
{
    QAction* tempAction = actionAt(event->pos());
    if (tempAction == nullptr) {
        mOverAction = nullptr;
        update();
        return;
    }
    if (tempAction->isChecked() || mOverAction == tempAction)
        return;
    mOverAction = tempAction;
    update();
    QWidget::mouseMoveEvent(event);
}

void SideBar::leaveEvent(QEvent* event)
{
    mOverAction = nullptr;
    update();
    QWidget::leaveEvent(event);
}

QAction* SideBar::actionAt(const QPoint& at)
{
    int action_y = 0;
    for (std::size_t idx = 0; idx < mActions.size(); ++idx) {
        QRect actionRect(0, action_y, rect().width(), rect().width());
        if (actionRect.contains(at)) {
            if (_strategy && idx != 0 && idx != 1) // only home/experiment enabled in strategy mode
                return nullptr;
            return mActions.at(idx);
        }
        action_y += actionRect.height();
    }
    return nullptr;
}

void SideBar::onHome()
{
    onSubframeChanged();
    gGui->_layout_stack->setCurrentIndex(static_cast<int>(SubFrame::Home));
    gGui->home->refreshTables();
}

void SideBar::onExperiment()
{
    onSubframeChanged();
    gGui->_layout_stack->setCurrentIndex(static_cast<int>(SubFrame::Experiment));
    gGui->experiment->refreshAll();
}

void SideBar::onFindPeaks()
{
    onSubframeChanged();
    gGui->_layout_stack->setCurrentIndex(static_cast<int>(SubFrame::Find));
    gGui->finder->refreshAll();
}

void SideBar::onFilterPeaks()
{
    onSubframeChanged();
    gGui->_layout_stack->setCurrentIndex(static_cast<int>(SubFrame::Filter));
    gGui->filter->refreshAll();
}

void SideBar::onIndexer()
{
    onSubframeChanged();
    gGui->_layout_stack->setCurrentIndex(static_cast<int>(SubFrame::Index));
    gGui->indexer->refreshAll();
}

void SideBar::onShapes()
{
    onSubframeChanged();
    gGui->_layout_stack->setCurrentIndex(static_cast<int>(SubFrame::Shapes));
    gGui->shapes->refreshAll();
}

void SideBar::onPredictor()
{
    onSubframeChanged();
    gGui->_layout_stack->setCurrentIndex(static_cast<int>(SubFrame::Predict));
    gGui->predictor->refreshAll();
}

void SideBar::onRefiner()
{
    onSubframeChanged();
    gGui->_layout_stack->setCurrentIndex(static_cast<int>(SubFrame::Refine));
    gGui->refiner->refreshAll();
}

void SideBar::onIntegrator()
{
    onSubframeChanged();
    gGui->_layout_stack->setCurrentIndex(static_cast<int>(SubFrame::Integrate));
    gGui->integrator->refreshAll();
}

void SideBar::onReject()
{
    onSubframeChanged();
    gGui->_layout_stack->setCurrentIndex(static_cast<int>(SubFrame::Reject));
    gGui->rejector->refreshAll();
}

void SideBar::onMerger()
{
    onSubframeChanged();
    gGui->_layout_stack->setCurrentIndex(static_cast<int>(SubFrame::Merge));
    gGui->merger->refreshSpaceGroupCombo();
    gGui->merger->processMerge();
}

void SideBar::refreshAll()
{
    gGui->finder->refreshAll();
    gGui->filter->refreshAll();
    gGui->indexer->refreshAll();
    gGui->predictor->refreshAll();
    gGui->refiner->refreshAll();
    gGui->integrator->refreshAll();
    gGui->merger->refreshAll();
    gGui->home->clearTables();
}

void SideBar::refreshCurrent()
{
    mCheckedAction->trigger();
}

void SideBar::onSubframeChanged()
{
    switch (static_cast<SubFrame>(gGui->_layout_stack->currentIndex())) {
    case SubFrame::Experiment: {
        gGui->experiment->setIndexerParameters();
        gGui->experiment->setStrategyParameters();
        break;
    }
    case SubFrame::Find: {
        gGui->finder->setFinderParameters();
        gGui->finder->setIntegrationParameters();
        break;
    }
    case SubFrame::Filter: {
        gGui->filter->setFilterParameters();
        break;
    }
    case SubFrame::Index: {
        gGui->indexer->setIndexerParameters();
        break;
    }
    case SubFrame::Shapes: {
        gGui->shapes->setShapeParameters();
        break;
    }
    case SubFrame::Predict: {
        gGui->predictor->setRefinerParameters();
        gGui->predictor->setPredictorParameters();
        gGui->predictor->setShapeModelParameters();
        break;
    }
    case SubFrame::Refine: {
        gGui->refiner->setRefinerParameters();
        break;
    }
    case SubFrame::Integrate: {
        gGui->integrator->setIntegrationParameters();
        break;
    }
    // SubframeMerge does not need setMergeParameters because it is triggered on merging
    }

    if (gSession->hasProject())
        gSession->currentProject()->writeYaml();

}
