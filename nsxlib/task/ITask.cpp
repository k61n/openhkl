#include <algorithm>
#include <random>
#include <set>

#include "ITask.h"

namespace nsx {

std::string generateRandomString(size_t length)
{
    std::string charmap("abcdefghijklmnopqrstuvwxyz");

    const size_t charmapLength = charmap.size();

    auto generator = [&](){ return charmap[rand()%charmapLength]; };

    std::string result;

    result.reserve(length);

    std::generate_n(back_inserter(result), length, generator);

    return result;
}

ITask::ITask(const std::string& type)
    : _abort(false),
      _callback(),
      _type(type),
      _current_step(0),
      _n_steps(0),
      _status(Status::SUBMITTED),
      _info()
{
    static std::set<std::string> registered_task_names;

    _name = generateRandomString(6);
    while (registered_task_names.find(_name) != registered_task_names.end()) {
        _name = generateRandomString(6);
    }
}

ITask::~ITask()
{
}

void ITask::abort()
{
    _abort = true;
}

void ITask::setCallBack(std::function<void(ITask*)> callback)
{
    _callback = callback;
}

TaskState ITask::state() const
{
    TaskState state;

    state.name = _name;
    state.type = _type;
    state.current_step = _current_step;
    state.n_steps = _n_steps;
    state.status = _status;
    state.info = _info;

    return state;
}

void ITask::sendState()
{
    if (!_callback) {
        return;
    }

    _callback(this);
}

bool ITask::run()
{
    _abort = false;

    bool success = doTask();

    return success;
}

} // end namespace nsx

