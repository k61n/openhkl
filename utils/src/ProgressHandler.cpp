#include "ProgressHandler.h"

ProgressHandler::ProgressHandler()
{

}

ProgressHandler::ProgressHandler(ProgressHandler::ProgressCallback callback)
{

}

void ProgressHandler::setMaximum(int max)
{

}

void ProgressHandler::update(int n)
{
    if (_callback) {
        _continue = _callback(n);
    }
}
