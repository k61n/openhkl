//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/SideBar.cpp
//! @brief     Implements class SideBar
//!
//! @homepage  ###HOMEPAGE###
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
#include "gui/subframe_combine/SubframeMergedPeaks.h"
#include "gui/subframe_experiment/PropertyPanel.h"
#include "gui/subframe_experiment/SubframeExperiment.h"
#include "gui/subframe_filter/SubframeFilterPeaks.h"
#include "gui/subframe_find/SubframeFindPeaks.h"
#include "gui/subframe_home/SubframeHome.h"
#include "gui/subframe_index/SubframeAutoIndexer.h"
#include "gui/subframe_integrate/SubframeIntegrate.h"
#include "gui/subframe_predict/SubframePredictPeaks.h"
#include "gui/subframe_refiner/SubframeRefiner.h"

#include <QDebug>
#include <QEvent>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QSignalBlocker>
#include <qapplication.h>

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
// merger.svg: Merge by Muneer A.Safiah from the Noun Project

SideBar::SideBar(QWidget* parent) : QWidget(parent), mCheckedAction(nullptr), mOverAction(nullptr)
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
    QAction* indexer = addAction(QIcon(path + QString("indexer.svg")), "Indexer");
    QAction* predictor = addAction(QIcon(path + QString("predictor.svg")), "Predict");
    QAction* refiner = addAction(QIcon(path + QString("refiner.svg")), "Refine");
    QAction* integrator = addAction(QIcon(path + QString("integrator.svg")), "Integrate");
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
    connect(predictor, &QAction::triggered, this, &SideBar::onPredictor);
    connect(refiner, &QAction::triggered, this, &SideBar::onRefiner);
    connect(integrator, &QAction::triggered, this, &SideBar::onIntegrator);
    connect(info, &QAction::triggered, this, &SideBar::onMerger);
}


void SideBar::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    QFont fontText(painter.font());
    fontText.setFamily("Helvetica Neue");
    painter.setFont(fontText);

    int action_y = 0;
    // painter.fillRect(rect(), QColor(100, 100, 100));
    for (auto action : mActions) {
        QRect actionRect(0, action_y, event->rect().width(), event->rect().width());

        if (action->isChecked()) {
            QColor fill_color;
            if (gGui->isDark())
                fill_color = QColor(35, 35, 35);
            else
                fill_color = QColor(200, 200, 200);
            painter.fillRect(actionRect, fill_color);
        }

        if (action == mOverAction)
            painter.fillRect(actionRect, QColor(150, 150, 150));

        if (gGui->isDark()) // looks like we have a dark theme
            painter.setPen(Qt::white);
        else
            painter.setPen(Qt::black);

        QSize size = painter.fontMetrics().size(Qt::TextSingleLine, action->text());
        QRect actionTextRect(
            QPoint(
                actionRect.width() / 2 - size.width() / 2, actionRect.bottom() - size.height() - 5),
            size);
        painter.drawText(actionTextRect, Qt::AlignCenter, action->text());

        QRect actionIconRect(
            0, action_y + 10, actionRect.width(),
            actionRect.height() - 2 * actionTextRect.height() - 10);
        QIcon actionIcon(action->icon());
        actionIcon.paint(&painter, actionIconRect);

        action_y += actionRect.height();
    } 
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
    action->blockSignals(false);
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
    for (auto action : mActions) {
        QRect actionRect(0, action_y, rect().width(), rect().width());
        if (actionRect.contains(at))
            return action;
        action_y += actionRect.height();
    }
    return nullptr;
}

void SideBar::onHome()
{
    gGui->_layout_stack->setCurrentIndex(0);
    gGui->home->refreshTables();
    emit subframeChanged();
}

void SideBar::onExperiment()
{
    gGui->_layout_stack->setCurrentIndex(1);
    if (gSession->hasProject()) {
        gGui->experiment->getProperty()->unitCellChanged();
        gGui->experiment->getProperty()->peaksChanged();
        gGui->experiment->getProperty()->experimentChanged();
        gGui->experiment->getProperty()->dataChanged();
    }
    emit subframeChanged();
}

void SideBar::onFindPeaks()
{
    gGui->_layout_stack->setCurrentIndex(2);
    gGui->finder->refreshAll();
    emit subframeChanged();
}

void SideBar::onFilterPeaks()
{
    gGui->_layout_stack->setCurrentIndex(3);
    gGui->filter->refreshAll();
    emit subframeChanged();
}

void SideBar::onIndexer()
{
    gGui->_layout_stack->setCurrentIndex(4);
    gGui->indexer->refreshAll();
    emit subframeChanged();
}

void SideBar::onPredictor()
{
    gGui->_layout_stack->setCurrentIndex(5);
    gGui->predictor->refreshAll();
    emit subframeChanged();
}

void SideBar::onRefiner()
{
    gGui->_layout_stack->setCurrentIndex(6);
    gGui->refiner->refreshAll();
    emit subframeChanged();
}

void SideBar::onIntegrator()
{
    gGui->_layout_stack->setCurrentIndex(7);
    gGui->integrator->refreshAll();
    emit subframeChanged();
}

void SideBar::onMerger()
{
    gGui->_layout_stack->setCurrentIndex(8);
    gGui->merger->refreshAll();
    emit subframeChanged();
}

void SideBar::refreshAll()
{
    if (gSession->currentProjectNum() != -1) {
        gGui->experiment->getProperty()->unitCellChanged();
        gGui->experiment->getProperty()->peaksChanged();
        gGui->experiment->getProperty()->experimentChanged();
        gGui->experiment->getProperty()->dataChanged();
    }
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