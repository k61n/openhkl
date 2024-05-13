//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/frames/ProgressView.cpp
//! @brief     Implements class ProgressView
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/frames/ProgressView.h"

#include <QApplication>
#include <QDebug>
#include <QPushButton>
#include <QTimer>
#include <QtGlobal>

ProgressView::ProgressView(QWidget* parent) : QProgressDialog(parent)
{
    setModal(true);
    setWindowTitle("Progress");

    setLabelText("Nothing to show");
    setMaximum(100);
    setValue(0);
    hide();

    QPushButton* cancel_button = new QPushButton("Cancel");
    cancel_button->setAutoDefault(false);
    setCancelButton(cancel_button);

    connect(this, SIGNAL(canceled()), this, SLOT(abort()));

    _timer.reset(new QTimer());
}

ProgressView::~ProgressView() = default;

void ProgressView::watch(ohkl::sptrProgressHandler handler)
{
    _handler = handler;

    _timer->stop();
    _timer->setInterval(200);

    connect(_timer.get(), SIGNAL(timeout()), this, SLOT(updateProgress()));
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
        qDebug() << "Status:" << _handler->getStatus().c_str() << " " << _handler->getProgress();

    std::vector<std::string> log = _handler->getLog();

    for (const std::string& msg : log)
        qDebug() << msg.c_str();
}

void ProgressView::abort()
{
    if (!_handler)
        return;

    _handler->abort();

    // call update to flush ouput log etc.
    updateProgress();

    _timer->stop();
}
