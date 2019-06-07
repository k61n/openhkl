#pragma once

#include <map>

#include <QWidget>

#include <core/DataTypes.h>
#include <core/Refiner.h>

namespace Ui {
class WidgetRefinerFit;
}

class WidgetRefinerFit : public QWidget {

    Q_OBJECT

public:
    WidgetRefinerFit(const std::map<nsx::sptrDataSet, nsx::Refiner>& refiners);

    ~WidgetRefinerFit();

private slots:

    void slotSelectedDataChanged(int selected_data);

    void slotSelectedBatchChanged(int selected_batch);

    void slotSelectedFrameChanged(int selected_frame);

private:
    Ui::WidgetRefinerFit* _ui;

    std::map<nsx::sptrDataSet, nsx::Refiner> _refiners;
};
