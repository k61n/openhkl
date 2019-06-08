//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/widgets/WidgetFoundPeaks.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QWidget>

#include "core/crystal/CrystalTypes.h"
#include "core/instrument/InstrumentTypes.h"

namespace Ui {
class WidgetFoundPeaks;
}

class ExperimentItem;
class SessionModel;

class WidgetFoundPeaks : public QWidget {

public:
    WidgetFoundPeaks(ExperimentItem* experiment_item, const nsx::PeakList& peaks);

    ~WidgetFoundPeaks();

    nsx::PeakList selectedPeaks() const;

private:
    Ui::WidgetFoundPeaks* _ui;

    ExperimentItem* _experiment_item;
};
