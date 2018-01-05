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
    DialogPeakFilter(const nsx::PeakSet& peaks, QWidget* parent=0);
    virtual ~DialogPeakFilter();
    const nsx::PeakSet& badPeaks() const;

public slots:
    virtual void accept() override;

private:
Ui::PeakFilterDialog* _ui;
    nsx::PeakSet _peaks;
    nsx::PeakSet _badPeaks;
};
