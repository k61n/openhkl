#ifndef LOGGER_H
#define LOGGER_H
#include <sstream>
class Logger
{
public:
    Logger();
    ~Logger();
    std::ostringstream& get();
private:
    Logger(const Logger&);
    Logger& operator=(const Logger&);
    std::ostringstream os;

};

#endif // LOGGER_H
