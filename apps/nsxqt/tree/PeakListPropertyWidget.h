#pragma once

#include <memory>
#include <QStandardItem>
#include <QWidget>

#include "PeakTableView.h"

namespace Ui {
class PeakListPropertyWidget;
}

class PeakListItem;

class PeakListPropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit PeakListPropertyWidget(PeakListItem* caller, QWidget *parent = 0);
    ~PeakListPropertyWidget();
    PeakTableView* getPeakTableView() const;

private:
    PeakListItem* _caller;
    Ui::PeakListPropertyWidget *ui;
};
