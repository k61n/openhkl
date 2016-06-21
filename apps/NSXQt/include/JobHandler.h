// author: Jonathan Fisher
// j.fisher@fz-juelich.de


#ifndef NSXTOOL_JOBHANDLER_H_
#define NSXTOOL_JOBHANDLER_H_

#include <functional>
#include <thread>
#include <map>

//! Class designed to handle a job schedule in a multi-threaded way
class JobHandler {
public:


    JobHandler();
    ~JobHandler();

    //! add a job to the queue, return id
    // int addJob(Job job);

private:
    std::map<int, std::thread> _jobQueue;

};

#endif // NSXTOOL_JOBHANDLER_H_
