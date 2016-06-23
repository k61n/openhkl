// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_PROGRESSVIEW_H_
#define NSXTOOL_PROGRESSVIEW_H_

#include <QObject>
#include <QProgressDialog>
#include <QTimer>

#include "ProgressHandler.h"

class ProgressView: public QObject {
    Q_OBJECT

public:
    ProgressView();
    ~ProgressView();

    void watch(SX::Utils::ProgressHandler* handler);

public slots:
    void update();

private:
    SX::Utils::ProgressHandler* _handler;
    QTimer* _timer;
    QProgressDialog* _dialog;
};



#endif
