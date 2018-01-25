#pragma once

#include <map>
#include <string>

#include <QGroupBox>
#include <QDialog>
#include <QVBoxLayout>

#include <nsxlib/DataTypes.h>
#include <nsxlib/PeakList.h>

namespace Ui {
class PeakFilterDialog;
}

class DialogPeakFilter : public QDialog {

public:
    DialogPeakFilter(const nsx::PeakList& peaks, QWidget* parent=0);
    virtual ~DialogPeakFilter();
    const nsx::PeakList& badPeaks() const;

public slots:
    virtual void accept() override;

private:
    Ui::PeakFilterDialog* _ui;
    nsx::PeakList _peaks;
    nsx::PeakList _badPeaks;
};
