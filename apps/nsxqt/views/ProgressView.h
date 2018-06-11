#pragma once

#include <memory>

#include <QObject>
#include <QProgressDialog>
#include <QTimer>

#include <nsxlib/UtilsTypes.h>

class ProgressView: public QProgressDialog {
    Q_OBJECT

public:
    ProgressView(QWidget* parent);
    ~ProgressView();

    void watch(nsx::sptrProgressHandler handler);

public slots:
    void updateProgress();
    void abort();

private:
    nsx::sptrProgressHandler _handler;
    QTimer* _timer;
};
