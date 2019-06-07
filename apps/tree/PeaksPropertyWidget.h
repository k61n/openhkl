#pragma once

#include <QStandardItem>
#include <QWidget>

#include "CollectedPeaksModel.h"

#include "PeakTableView.h"

namespace Ui {
class PeaksPropertyWidget;
}

class PeaksItem;

class PeaksPropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit PeaksPropertyWidget(PeaksItem* caller, QWidget* parent = 0);

    ~PeaksPropertyWidget();

    CollectedPeaksModel* model();

private:
    PeaksItem* _peaks_item;

    Ui::PeaksPropertyWidget* ui;

    CollectedPeaksModel* _peaks_model;
};
