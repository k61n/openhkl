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
#include "gui/MainWin.h"
#include "gui/models/Session.h"
#include "gui/subframe_combine/SubframeMergedPeaks.h"
#include "gui/subframe_experiment/PropertyPanel.h"
#include "gui/subframe_experiment/SubframeExperiment.h"
#include "gui/subframe_filter/SubframeFilterPeaks.h"
#include "gui/subframe_find/SubframeFindPeaks.h"
#include "gui/subframe_home/SubframeHome.h"
#include "gui/subframe_index/SubframeAutoIndexer.h"
#include "gui/subframe_predict/SubframePredictPeaks.h"
#include "gui/subframe_refiner/SubframeRefiner.h"

#include <QDebug>
#include <QEvent>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>

#define action_height 100


SideBar::SideBar(QWidget* parent) : QWidget(parent), mCheckedAction(nullptr), mOverAction(nullptr)
{
    setMouseTracking(true);

    QAction* home = addAction(QIcon(":/images/home.svg"), "Home");
    QAction* experiment = addAction(QIcon(":/images/experiment_info.svg"), "Experiment");
    QAction* finder = addAction(QIcon(":/images/find_peaks.svg"), "Find Peaks");
    QAction* filter = addAction(QIcon(":/images/filterIcon.svg"), "Peak Filter");
    QAction* indexer = addAction(QIcon(":/images/uni_cell.svg"), "Indexer");
    QAction* predictor = addAction(QIcon(":/images/predict_peaks.svg"), "Predictor");
    QAction* refiner = addAction(QIcon(":/images/filterIcon.svg"), "Refiner");
    QAction* info = addAction(QIcon(":/images/merge.svg"), "Merger");

    QAction* tempAction = mActions.at(0);
    mCheckedAction = tempAction;
    tempAction->setChecked(true);
    update();
    ;

    connect(home, &QAction::triggered, this, &SideBar::onHome);
    connect(experiment, &QAction::triggered, this, &SideBar::onExperiment);
    connect(finder, &QAction::triggered, this, &SideBar::onFindPeaks);
    connect(filter, &QAction::triggered, this, &SideBar::onFilterPeaks);
    connect(indexer, &QAction::triggered, this, &SideBar::onIndexer);
    connect(predictor, &QAction::triggered, this, &SideBar::onPredictor);
    connect(refiner, &QAction::triggered, this, &SideBar::onRefiner);
    connect(info, &QAction::triggered, this, &SideBar::onMerger);
}

void SideBar::paintEvent(QPaintEvent* event)
{
    QPainter p(this);

    QFont fontText(p.font());
    fontText.setFamily("Helvetica Neue");
    p.setFont(fontText);

    int action_y = 0;
    // p.fillRect(rect(), QColor(100, 100, 100));
    for (auto action : mActions) {
        QRect actionRect(0, action_y, event->rect().width(), action_height);

        if (action->isChecked())
            p.fillRect(actionRect, QColor(35, 35, 35));

        if (action == mOverAction)
            p.fillRect(actionRect, QColor(150, 150, 150));

        p.setPen(QColor(255, 255, 255));
        QSize size = p.fontMetrics().size(Qt::TextSingleLine, action->text());
        QRect actionTextRect(
            QPoint(
                actionRect.width() / 2 - size.width() / 2, actionRect.bottom() - size.height() - 5),
            size);
        p.drawText(actionTextRect, Qt::AlignCenter, action->text());

        QRect actionIconRect(
            0, action_y + 10, actionRect.width(),
            actionRect.height() - 2 * actionTextRect.height() - 10);
        QIcon actionIcon(action->icon());
        actionIcon.paint(&p, actionIconRect);

        action_y += actionRect.height();
    }
}

QSize SideBar::minimumSizeHint() const
{
    return action_height * QSize(1, mActions.size());
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
    for (auto action : mActions) {
        QRect actionRect(0, action_y, rect().width(), action_height);
        if (actionRect.contains(at))
            return action;
        action_y += actionRect.height();
    }
    return nullptr;
}

void SideBar::onHome()
{
    gGui->_layout_stack->setCurrentIndex(0);
}

void SideBar::onExperiment()
{
    gGui->_layout_stack->setCurrentIndex(1);
    if (gSession->currentProjectNum() != -1) {
        gGui->experiment->getProperty()->unitCellChanged();
        gGui->experiment->getProperty()->peaksChanged();
        gGui->experiment->getProperty()->experimentChanged();
        gGui->experiment->getProperty()->dataChanged();
    }
}

void SideBar::onFindPeaks()
{
    gGui->_layout_stack->setCurrentIndex(2);
    gGui->finder->refreshAll();
}

void SideBar::onFilterPeaks()
{
    gGui->_layout_stack->setCurrentIndex(3);
    gGui->filter->refreshAll();
}

void SideBar::onIndexer()
{
    gGui->_layout_stack->setCurrentIndex(4);
    if (gSession->currentProjectNum() != -1)
        gGui->indexer->refreshAll();
}

void SideBar::onPredictor()
{
    gGui->_layout_stack->setCurrentIndex(5);
    gGui->predictor->refreshAll();
}

void SideBar::onRefiner()
{
    gGui->_layout_stack->setCurrentIndex(6);
    gGui->refiner->refreshAll();
}

void SideBar::onMerger()
{
    gGui->_layout_stack->setCurrentIndex(7);
    gGui->merger->refreshAll();
}

#undef action_height
