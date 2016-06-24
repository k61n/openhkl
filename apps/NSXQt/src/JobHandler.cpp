// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include "JobHandler.h"


WorkerThread::WorkerThread(QObject *parent, std::function<void(void)> task): QThread(parent), _task(task)
{

}

void WorkerThread::run()
{
    if ( _task )
        _task();

    emit resultReady();
}

Job::Job(QObject *parent, std::function<void(void)> task, std::function<void(void)> onFinished):
    QObject(parent), _onFinished(onFinished)
{
    WorkerThread *workerThread = new WorkerThread(this, task);
    connect(workerThread, SIGNAL(resultReady()), this, SLOT(resultReady()));
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    workerThread->start();
}

void Job::resultReady()
{
    if (_onFinished)
        _onFinished();
}
