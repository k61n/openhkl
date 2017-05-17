// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include <QDebug>
#include <QApplication>

#include "ProgressView.h"

ProgressView::ProgressView(QWidget* parent): QProgressDialog(parent)
{
    setModal(true);

    setLabelText("Nothing to show");
    setMaximum(100);
    setValue(0);
    hide();

    connect(this, SIGNAL(canceled()), this, SLOT(abort()));

    _timer = new QTimer();
}

ProgressView::~ProgressView()
{
    _timer->stop();

    delete _timer;
    _timer = nullptr;

}

void ProgressView::watch(std::shared_ptr<nsx::ProgressHandler> handler)
{
    _handler = handler;

    _timer->stop();
    _timer->setInterval(200);

    connect(_timer, SIGNAL(timeout()), this, SLOT(updateProgress()));
    connect(this, SIGNAL(canceled()), this, SLOT(abort()));

    _timer->start();

    _handler->setCallback([] () {QApplication::processEvents();});
}

void ProgressView::updateProgress()
{
    if (!_handler)
        return;

    // dialog was cancelled or otherwise failed
    if (_handler->aborted())
            return;

    int progress = _handler->getProgress();

    /*
    if ( progress > 0 && progress < 100 )
        _dialog->show();
    else
        _dialog->hide();
    */


    setLabelText(_handler->getStatus().c_str());
    setValue(progress);

    if ( progress < 0)
        qDebug() << "Status:" << _handler->getStatus().c_str() << " " << _handler->getProgress();

    auto log = _handler->getLog();

    for (auto& msg: log)
        qDebug() << msg.c_str();
}

void ProgressView::abort()
{
    if (!_handler)
        return;

    _handler->abort();

    // call update to flush ouput log etc.
    updateProgress();

    qDebug() << "Job was aborted.";
    _timer->stop();
}
