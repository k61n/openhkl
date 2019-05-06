
#include "gui/view/toggles.h"

Toggles::Toggles()
{
    //cursor modes
    pixelPosition.setCheckable(true);
    gammaNu.setCheckable(true);
    twoTheta.setCheckable(true);
    dSpacing.setCheckable(true);
    millerIndices.setCheckable(true);

    QAction::connect(&pixelPosition, &QAction::toggled, [this]()->void{
                         bool check = this->pixelPosition.isChecked();
                         if (check) {
                             this->gammaNu.setChecked(!check);
                             this->twoTheta.setChecked(!check);
                             this->dSpacing.setChecked(!check);
                             this->millerIndices.setChecked(!check);
                         }
                     });

    //setView
    fromSample.setCheckable(true);
    behindDetector.setCheckable(true);
    logarithmicScale.setCheckable(true);

    //peak
    showLabels.setCheckable(true);
    showAreas.setCheckable(true);
    drawPeakArea.setCheckable(true);
}
