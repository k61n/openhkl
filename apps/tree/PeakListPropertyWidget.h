#pragma once

#include <memory>

#include <QStandardItem>
#include <QWidget>

#include "CollectedPeaksModel.h"
#include "PeakTableView.h"

namespace Ui {
class PeakListPropertyWidget;
}

class PeakListItem;

class PeakListPropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit PeakListPropertyWidget(PeakListItem* caller, QWidget* parent = 0);

    ~PeakListPropertyWidget();

    CollectedPeaksModel* model();

private:
    PeakListItem* _caller;

    Ui::PeakListPropertyWidget* ui;

    CollectedPeaksModel* _peaks_model;
};
