// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include "JobHandler.h"

using TaskCallback = WorkerThread::TaskCallback;
using FinishedCallback = WorkerThread::FinishedCallback;


WorkerThread::WorkerThread(QObject *parent, TaskCallback task): QThread(parent), _task(task)
{

}

void WorkerThread::run()
{
    bool success = false;

    try {
        if ( _task )
            success = _task();
    }
    catch(...) {
        emit resultReady(false);
    }

    emit resultReady(success);
}

Job::Job(QObject *parent, TaskCallback task, FinishedCallback onFinished):
    QObject(parent), _task(task), _onFinished(onFinished)
{
    _workerThread = new WorkerThread(this, task);
    connect(_workerThread, SIGNAL(resultReady(bool)), this, SLOT(resultReady(bool)));
    connect(_workerThread, SIGNAL(finished()), _workerThread, SLOT(deleteLater()));
    connect(this, SIGNAL(terminateThread()), _workerThread, SLOT(terminate()));
}

void Job::exec()
{
    // execute asynchronously in another thread
    _workerThread->start();
}

void Job::resultReady(bool success)
{
    if (_onFinished)
        _onFinished(success);
}

void Job::terminated()
{
    if (_onFinished)
        _onFinished(false);
}

void Job::terminate()
{
    emit terminateThread();
}

