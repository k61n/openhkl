// author: Jonathan Fisher
// j.fisher@fz-juelich.de


#ifndef NSXTOOL_JOBHANDLER_H_
#define NSXTOOL_JOBHANDLER_H_


#include <QObject>
#include <QThread>

#include <functional>
#include <memory>


class WorkerThread : public QThread
{
    Q_OBJECT
public:
    using TaskCallback = std::function<bool(void)>;
    using FinishedCallback = std::function<void(bool)>;

    WorkerThread(QObject* parent, TaskCallback task);
    void run();

signals:
    void resultReady(bool success);

private:
    TaskCallback _task;
};


class Job : public QObject
{
    Q_OBJECT

public:
    Job(QObject* parent, WorkerThread::TaskCallback task, WorkerThread::FinishedCallback onFinished);
    void exec();

public slots:
    void resultReady(bool success);
    void terminated();
    void terminate();

signals:
    void terminateThread();

private:
    std::unique_ptr<WorkerThread> _worker;
    WorkerThread::FinishedCallback _onFinished;
    WorkerThread* _workerThread;
};

#endif // NSXTOOL_JOBHANDLER_H_
