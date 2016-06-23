// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include "ProgressView.h"

ProgressView::ProgressView()
{

}

ProgressView::~ProgressView()
{
    if ( _timer ) {
        _timer->stop();
        delete _timer;
        _timer = nullptr;
    }

    if ( _dialog ) {
        delete _dialog;
        _dialog = nullptr;
    }
}

void ProgressView::watch(SX::Utils::ProgressHandler *handler)
{
    _handler = handler;

    if ( _timer )
        delete _timer;

    _timer = new QTimer(this);
    _timer->setInterval(200);

    connect(_timer, SIGNAL(timeout()), this, SLOT(update()));

    _timer->start();
}

void ProgressView::update()
{
    if (!_handler)
        return;

    if (!_dialog) {
        _dialog = new QProgressDialog();
    }

    int progress = _handler->getProgress();

    if ( progress < 100 )
        _dialog->show();
    else
        _dialog->hide();

    _dialog->setLabelText(_handler->getStatus().c_str());
    _dialog->setValue(progress);
}
