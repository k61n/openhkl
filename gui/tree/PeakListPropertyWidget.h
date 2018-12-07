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

    explicit PeakListPropertyWidget(PeakListItem* peak_list_item, QWidget *parent = 0);

    ~PeakListPropertyWidget();

    CollectedPeaksModel* model();

private:
    PeakListItem* _peak_list_item;

    Ui::PeakListPropertyWidget *_ui;
};
