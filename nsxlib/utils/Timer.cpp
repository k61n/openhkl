#include "Timer.h"

namespace SX {


Timer::Timer():_bt(0),_et(0)
{

}

Timer::~Timer()
{

}

void Timer::start()
{
    _bt=clock();
}

void Timer::stop()
{
    _et=clock();
}

std::ostream& operator<<(std::ostream& os,const Timer& t)
{
    os << static_cast<double>(t._et-t._bt)/CLOCKS_PER_SEC << " sec. ";
    return os;
}

} // end namespace SX
