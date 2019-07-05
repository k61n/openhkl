//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/ProgressView.cpp
//! @brief     Implements class ProgressView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/frames/ProgressView.h"

#include "base/utils/ProgressHandler.h"
#include <QApplication>
#include <QDebug>
#include <QPushButton>
#include <QtGlobal>

ProgressView::ProgressView(QWidget* parent) : QProgressDialog(parent)
{
    setModal(true);

    setLabelText("Nothing to show");
    setMaximum(100);
    setValue(0);
    hide();

    QPushButton* cancel_button = new QPushButton("Cancel");
    cancel_button->setAutoDefault(false);
    setCancelButton(cancel_button);

    connect(this, SIGNAL(canceled()), this, SLOT(abort()));

    _timer = new QTimer();
}

ProgressView::~ProgressView()
{
    _timer->stop();

    delete _timer;
    _timer = nullptr;
}

void ProgressView::watch(nsx::sptrProgressHandler handler)
{
    _handler = handler;

    _timer->stop();
    _timer->setInterval(200);

    connect(_timer, SIGNAL(timeout()), this, SLOT(updateProgress()));
    connect(this, SIGNAL(canceled()), this, SLOT(abort()));

    _timer->start();

    _handler->setCallback([]() { QApplication::processEvents(); });
}

void ProgressView::updateProgress()
{
    if (!_handler)
        return;

    // dialog was cancelled or otherwise failed
    if (_handler->aborted())
        return;

    int progress = _handler->getProgress();

    setLabelText(_handler->getStatus().c_str());
    setValue(progress);

    if (progress < 0)
        qInfo() << "Status:" << _handler->getStatus().c_str() << " " << _handler->getProgress();

    std::vector<std::string> log = _handler->getLog();

    for (std::string msg : log)
        qInfo() << msg.c_str();
}

void ProgressView::abort()
{
    if (!_handler)
        return;

    _handler->abort();

    // call update to flush ouput log etc.
    updateProgress();

    qInfo() << "Job was aborted.";
    _timer->stop();
}
