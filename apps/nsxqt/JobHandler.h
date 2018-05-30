#pragma once

#include <memory>

#include <QObject>
#include <QThread>

#include <nsxlib/UtilsTypes.h>

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread(QObject* parent, nsx::TaskCallback task);
    void run();

signals:
    void resultReady(bool success);

private:
    nsx::TaskCallback _task;
};


class Job : public QObject
{
    Q_OBJECT

public:
    Job(QObject* parent,
        nsx::TaskCallback task,
        nsx::FinishedCallback onFinished,
        bool executeSynchronous);
    void exec();

public slots:
    void resultReady(bool success);
    void terminated();
    void terminate();

signals:
    void terminateThread();

private:
    std::unique_ptr<WorkerThread> _worker;
    nsx::TaskCallback _task;
    nsx::FinishedCallback _onFinished;
    WorkerThread* _workerThread;
    bool _synchronous;
};
