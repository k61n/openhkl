#include <QApplication>
#include <QPushButton>

#include <core/Logger.h>
#include <core/ProgressHandler.h>

#include "ProgressView.h"

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
        nsx::info() << "Status:" << _handler->getStatus().c_str() << " " << _handler->getProgress();

    auto log = _handler->getLog();

    for (auto& msg : log)
        nsx::info() << msg.c_str();
}

void ProgressView::abort()
{
    if (!_handler)
        return;

    _handler->abort();

    // call update to flush ouput log etc.
    updateProgress();

    nsx::info() << "Job was aborted.";
    _timer->stop();
}
