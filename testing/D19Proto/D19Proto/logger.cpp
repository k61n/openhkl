#include "logger.h"

Logger::Logger()
{
}
Logger::~Logger()
{

}

std::ostringstream& Logger::get()
{
    os << "-----------------------------" << std::endl;
    return os;
}
