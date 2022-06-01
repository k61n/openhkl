//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/utils/Units.cpp
//! @brief     Implements class UnitsManager
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/utils/Units.h"
#include <stdexcept>

namespace {
using namespace nsx;

static std::unordered_map<std::string, double> unitsMap = {
    {"m", m},
    {"cm", cm},
    {"mm", mm},
    {"um", um},
    {"nm", nm},
    {"ang", ang},
    {"pm", pm},
    {"fm", fm},
    {"am", am},
    {"m2", m2},
    {"cm2", cm2},
    {"mm2", mm2},
    {"um2", um2},
    {"nm2", nm2},
    {"pm2", pm2},
    {"am2", am2},
    {"barn", barn},
    {"m3", m3},
    {"cm3", cm3},
    {"mm3", mm3},
    {"um3", um3},
    {"nm3", nm3},
    {"pm3", pm3},
    {"fm3", fm3},
    {"am3", am3},
    {"g", g},
    {"rad", rad},
    {"deg", deg},
    {"mrad", mrad},
    {"sr", sr},
    {"s", s},
    {"ms", ms},
    {"us", us},
    {"min", min},
    {"hour", hour},
    {"Pa", Pa},
    {"Bar", Bar},
    {"day", day},
    {"year", year},
    {"au", au},
    {"%", perCent},
    {"perCent", perCent},
    {"percentage", perCent},
    {"avogadro", avogadro},
    {"uma", uma},
    {"g_per_mole", g_per_mole},
    {"g/cm3", g_per_cm3},
    {"kg/m3", kg_per_m3},
    {"R", R}};

} // namespace

namespace nsx {

double UnitsManager::get(const std::string& units)
{
    auto it = unitsMap.find(units);
    if (it == unitsMap.end())
        throw std::invalid_argument("Unit " + units + " not registered in the database");
    return it->second;
}

} // namespace nsx
