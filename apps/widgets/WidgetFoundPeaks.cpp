#include <core/Experiment.h>
#include <core/Peak3D.h>

#include "CollectedPeaksModel.h"
#include "ExperimentItem.h"
#include "SessionModel.h"
#include "WidgetFoundPeaks.h"

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
