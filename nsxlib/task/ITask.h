#pragma once

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <string>

namespace nsx {

struct TaskState;

class ITask
{
public:

    enum class Status {SUBMITTED,STARTED,ABORTED,COMPLETED};

    ITask(const std::string& type);

    ITask(const ITask& other)=delete;

    ITask& operator=(const ITask& other)=delete;

    virtual ~ITask()=0;

    TaskState state() const;
    void sendState();

    void abort();

    bool run();

    virtual bool doTask()=0;

    void setCallBack(std::function<void(ITask*)> callback);

protected:

    std::atomic<bool> _abort;

    std::function<void(ITask*)> _callback;

    std::string _name;

    std::string _type;

    int _current_step;

    int _n_steps;

    Status _status;

    std::string _info;
};

struct TaskState {

    enum class Section : size_t {NAME=0,PROGRESS=1,START=2,ABORT=3,REMOVE=4,COUNT=5};

    std::string name;
    std::string type;
    int current_step;
    int n_steps;
    ITask::Status status;
    std::string info;
};

} // end namespace nsx
