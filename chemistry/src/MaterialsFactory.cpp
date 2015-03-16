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
	registerCallback(BuildingMode::MassFractions   ,&MaterialFromMassFractions::create);
	registerCallback(BuildingMode::MolarFractions  ,&MaterialFromMolarFractions::create);
	registerCallback(BuildingMode::PartialPressures,&MaterialFromPartialPressures::create);
	registerCallback(BuildingMode::Stoichiometry   ,&MaterialFromStoichiometry::create);
}

MaterialsFactory::~MaterialsFactory() {
}

} // end namespace Chemistry

} // end namespace SX
