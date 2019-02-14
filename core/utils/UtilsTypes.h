#pragma once

#include <functional>
#include <memory>
#include <set>
#include <tuple>

namespace nsx {

class ProgressHandler;

using sptrProgressHandler = std::shared_ptr<ProgressHandler>;

using ConstraintTuple = std::tuple<unsigned int, unsigned int, double>;
using ConstraintSet = std::set<ConstraintTuple>;

using FinishedCallback = std::function<void(bool)>;
using TaskCallback = std::function<bool(void)>;

} // end namespace nsx
