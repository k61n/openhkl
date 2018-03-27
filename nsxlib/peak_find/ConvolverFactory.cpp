#include "AnnularConvolver.h"
#include "ConvolverFactory.h"
#include "EnhancedAnnularConvolver.h"
#include "ConstantConvolver.h"
#include "DeltaConvolver.h"
#include "RadialConvolver.h"

namespace nsx {

template <typename T>
sptrConvolver create_convolver(const std::map<std::string,double>& parameters)
{
    return std::make_shared<T>(parameters);
}

ConvolverFactory::ConvolverFactory(): _callbacks()
{
    _callbacks["annular"] = &create_convolver<AnnularConvolver>;
    _callbacks["constant"] = &create_convolver<ConstantConvolver>;
    _callbacks["delta"] = &create_convolver<DeltaConvolver>;
    _callbacks["enhanced_annular"] = &create_convolver<EnhancedAnnularConvolver>;
    _callbacks["none"] = &create_convolver<DeltaConvolver>;
    _callbacks["radial"] = &create_convolver<RadialConvolver>;
}

sptrConvolver ConvolverFactory::create(const std::string& convolver_type, const std::map<std::string,double>& parameters) const
{
    const auto it = _callbacks.find(convolver_type);

    // could not find key
    if (it == _callbacks.end()) {
        throw std::runtime_error(convolver_type + " is not registered as a valid convolver");
    }

    return (it->second)(parameters);
}

const std::map<std::string,ConvolverFactory::callback>& ConvolverFactory::callbacks() const
{
    return _callbacks;
}

} // end namespace nsx

