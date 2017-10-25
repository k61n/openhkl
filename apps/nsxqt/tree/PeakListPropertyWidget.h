#ifndef NSXQT_PEAKLISTPROPERTYWIDGET_H
#define NSXQT_PEAKLISTPROPERTYWIDGET_H


#include <memory>
#include <QStandardItem>
#include <QWidget>

#include "views/PeakTableView.h"
#include "models/SessionModel.h"

namespace Ui {
class PeakListPropertyWidget;
}

class PeakListItem;

class PeakListPropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit PeakListPropertyWidget(std::shared_ptr<SessionModel> session, PeakListItem* caller, QWidget *parent = 0);
    ~PeakListPropertyWidget();
    PeakTableView* getPeakTableView() const;

private:
    PeakListItem* _caller;
    Ui::PeakListPropertyWidget *ui;
    std::shared_ptr<SessionModel> _session;
};

#endif // NSXQT_PEAKLISTPROPERTYWIDGET_H
