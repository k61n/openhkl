//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/widgets/WidgetFoundPeaks.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/Experiment.h"
#include "core/peak/Peak3D.h"

#include "apps/models/CollectedPeaksModel.h"
#include "apps/models/ExperimentItem.h"
#include "apps/models/SessionModel.h"
#include "apps/widgets/WidgetFoundPeaks.h"

#include "ui_WidgetFoundPeaks.h"

WidgetFoundPeaks::WidgetFoundPeaks(ExperimentItem* experiment_item, const nsx::PeakList& peaks)
    : _ui(new Ui::WidgetFoundPeaks)
{
    _ui->setupUi(this);

    CollectedPeaksModel* peaks_model =
        new CollectedPeaksModel(experiment_item->model(), experiment_item->experiment(), peaks);
    _ui->peaks->setModel(peaks_model);
}

WidgetFoundPeaks::~WidgetFoundPeaks()
{
    delete _ui;
}

nsx::PeakList WidgetFoundPeaks::selectedPeaks() const
{
    auto peaks_model = dynamic_cast<CollectedPeaksModel*>(_ui->peaks->model());

    auto peaks = peaks_model->peaks();

    nsx::PeakList found_peaks;

    if (!peaks.empty()) {
        found_peaks.reserve(peaks.size());
        for (auto peak : peaks) {
            if (_ui->keep_selected_peaks->isChecked()) {
                if (peak->selected()) {
                    found_peaks.push_back(peak);
                }
            } else {
                found_peaks.push_back(peak);
            }
        }
    }

    return found_peaks;
}
