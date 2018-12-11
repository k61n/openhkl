#pragma once

#include <QMetaType>

#include <string>

#include <core/PeakList.h>
#include <core/DataTypes.h>
#include <core/UnitCell.h>

Q_DECLARE_METATYPE(nsx::PeakList)
Q_DECLARE_METATYPE(nsx::sptrUnitCell)
Q_DECLARE_METATYPE(nsx::sptrDataSet)

Q_DECLARE_METATYPE(std::string)
