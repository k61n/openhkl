// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXQT_PROGRESSVIEW_H
#define NSXQT_PROGRESSVIEW_H

#include <memory>

#include <QObject>
#include <QProgressDialog>
#include <QTimer>

#include <nsxlib/utils/ProgressHandler.h>

class ProgressView: public QProgressDialog {
    Q_OBJECT

public:
    ProgressView(QWidget* parent);
    ~ProgressView();

    void watch(std::shared_ptr<nsx::ProgressHandler> handler);

public slots:
    void updateProgress();
    void abort();

private:
    std::shared_ptr<nsx::ProgressHandler> _handler;
    QTimer* _timer;
};



#endif // NSXQT_PROGRESSVIEW_H
