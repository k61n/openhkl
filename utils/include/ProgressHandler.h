#ifndef PROGRESSHANDLER_H
#define PROGRESSHANDLER_H

#include <functional>

class ProgressHandler
{
public:
    typedef std::function<bool(int)> ProgressCallback;

    ProgressHandler();
    ProgressHandler(ProgressCallback callback);

    void setMaximum(int max);
    void update(int n);

private:
    ProgressCallback _callback;
    int _maximum;
    bool _continue;
};


#endif // PROGRESSHANDLER_H
