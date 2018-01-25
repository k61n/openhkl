#include <stdexcept>

#include "PeakList.h"

namespace nsx {

PeakList::PeakList(const std::initializer_list<sptrPeak3D>& peaks) : _peaks(peaks)
{
}

sptrPeak3D PeakList::operator[](int index)
{
    if (index < 0 || index >= _peaks.size()) {
        throw std::runtime_error("Invalid peak index");
    }

    return _peaks[index];
}

const sptrPeak3D PeakList::operator[](int index) const
{
    if (index < 0 || index >= _peaks.size()) {
        throw std::runtime_error("Invalid peak index");
    }

    return _peaks[index];
}

void PeakList::reserve(size_t capacity)
{
    return _peaks.reserve(capacity);
}

bool PeakList::empty() const
{
    return _peaks.empty();
}

void PeakList::clear()
{
    _peaks.clear();
}

size_t PeakList::size() const
{
    return _peaks.size();
}

PeakList::peak_list_iterator PeakList::begin()
{
    return _peaks.begin();
}

PeakList::peak_list_const_iterator PeakList::begin() const
{
    return _peaks.cbegin();
}

PeakList::peak_list_iterator PeakList::end()
{
    return _peaks.end();
}

PeakList::peak_list_const_iterator PeakList::end() const
{
    return _peaks.cend();
}

PeakList::peak_list_const_iterator PeakList::cbegin() const
{
    return _peaks.cbegin();
}

PeakList::peak_list_const_iterator PeakList::cend() const
{
    return _peaks.cend();
}

void PeakList::add(sptrPeak3D peak)
{
    auto it = std::find(_peaks.begin(),_peaks.end(),peak);
    if (it != _peaks.end()) {
        return;
    }

    _peaks.push_back(peak);
}

void PeakList::remove(sptrPeak3D peak)
{
    auto it = std::find(_peaks.begin(),_peaks.end(),peak);
    if (it == _peaks.end()) {
        return;
    }

    _peaks.erase(it);
}

} // end namespace nsx

