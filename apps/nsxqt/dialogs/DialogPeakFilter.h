#pragma once

#include <map>
#include <string>

#include <QGroupBox>
#include <QDialog>
#include <QVBoxLayout>

#include <nsxlib/DataTypes.h>

namespace Ui {
class PeakFilterDialog;
}

class DialogPeakFilter : public QDialog {

public:
    DialogPeakFilter(const nsx::DataList& data, QWidget* parent=0);
    virtual ~DialogPeakFilter();

public slots:
    virtual void accept() override;

private:
    nsx::DataList _data;
    Ui::PeakFilterDialog* _ui;
};
