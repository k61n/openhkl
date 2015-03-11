#include "MaterialFromMassFractions.h"
#include "MaterialFromMolarFractions.h"
#include "MaterialFromPartialPressures.h"
#include "MaterialFromStoichiometry.h"
#include "MaterialsFactory.h"

namespace SX
{

namespace Chemistry
{

MaterialsFactory::MaterialsFactory()
{
	registerCallback(IMaterial::BuildingMode::MassFractions   ,&MaterialFromMassFractions::create);
	registerCallback(IMaterial::BuildingMode::MolarFractions  ,&MaterialFromMolarFractions::create);
	registerCallback(IMaterial::BuildingMode::PartialPressures,&MaterialFromPartialPressures::create);
	registerCallback(IMaterial::BuildingMode::Stoichiometry   ,&MaterialFromStoichiometry::create);
}

MaterialsFactory::~MaterialsFactory() {
}

} // end namespace Chemistry

} // end namespace SX
