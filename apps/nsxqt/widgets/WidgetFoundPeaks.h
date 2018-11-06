#pragma once

#include <QWidget>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/InstrumentTypes.h>

namespace Ui {
class WidgetFoundPeaks;
}

class ExperimentItem;
class SessionModel;

class WidgetFoundPeaks : public QWidget
{

public:

    WidgetFoundPeaks(ExperimentItem* experiment_item, const nsx::PeakList &peaks);

    ~WidgetFoundPeaks();

    nsx::PeakList selectedPeaks() const;

private:

    Ui::WidgetFoundPeaks *_ui;

    ExperimentItem *_experiment_item;
};
