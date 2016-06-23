// author: Jonathan Fisher
// j.fisher@fz-juelich.de


#ifndef NSXTOOL_JOBHANDLER_H_
#define NSXTOOL_JOBHANDLER_H_


//! Class designed to handle a job schedule in a multi-threaded way
class JobHandler {
public:
    JobHandler();
    ~JobHandler();

    //! add a job to the queue, return id
    // int addJob(Job job);

private:
    //QThread* _thread;

};

#endif // NSXTOOL_JOBHANDLER_H_
