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
    WorkerThread(QObject* parent, std::function<void(void)> task);
    void run();

signals:
    void resultReady();

private:
    std::function<void(void)> _task;
};


class Job : public QObject
{
    Q_OBJECT

public:
    Job(QObject* parent, std::function<void(void)> task, std::function<void(void)> onFinished);

public slots:
    void resultReady();

private:
    std::unique_ptr<WorkerThread> _worker;
    std::function<void(void)> _onFinished;
};

#endif // NSXTOOL_JOBHANDLER_H_
