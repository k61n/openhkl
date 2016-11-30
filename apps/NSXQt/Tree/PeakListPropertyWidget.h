#ifndef PEAKLISTPROPERTYWIDGET_H
#define PEAKLISTPROPERTYWIDGET_H

#include <QStandardItem>
#include <QWidget>
#include "PeakTableView.h"

namespace Ui {
class PeakListPropertyWidget;
}

class PeakListItem;

class PeakListPropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PeakListPropertyWidget(PeakListItem* caller, QWidget *parent = 0);
    ~PeakListPropertyWidget();
    PeakTableView* getPeakTableView() const;

private slots:

    void changeSelectedCells(QStringList);

    void on_obsPeaks_clicked(bool checked);

private:
    PeakListItem* _caller;
    Ui::PeakListPropertyWidget *ui;
};

#endif // PEAKLISTPROPERTYWIDGET_H
