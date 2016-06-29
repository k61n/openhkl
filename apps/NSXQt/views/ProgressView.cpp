// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include <QDebug>

#include "ProgressView.h"

ProgressView::ProgressView()
{
    _dialog = new QProgressDialog();


    _dialog->setLabelText("Nothing to show");
    _dialog->setMaximum(100);
    _dialog->setValue(0);
    _dialog->hide();


    _timer = new QTimer();
}

ProgressView::~ProgressView()
{
    _timer->stop();

    delete _timer;
    delete _dialog;

    _timer = nullptr;
    _dialog = nullptr;
}

void ProgressView::watch(std::shared_ptr<SX::Utils::ProgressHandler> handler)
{
    _handler = handler;

    _timer->stop();
    _timer->setInterval(200);

    connect(_timer, SIGNAL(timeout()), this, SLOT(update()));

    _timer->start();
}

void ProgressView::update()
{
    if (!_handler)
        return;

    int progress = _handler->getProgress();

    if ( progress > 0 && progress < 100 )
        _dialog->show();
    else
        _dialog->hide();

    _dialog->setLabelText(_handler->getStatus().c_str());
    _dialog->setValue(progress);

    if ( progress < 0)
        qDebug() << "Status:" << _handler->getStatus().c_str() << " " << _handler->getProgress();

    auto log = _handler->getLog();

    for (auto& msg: log)
        qDebug() << msg.c_str();
}
