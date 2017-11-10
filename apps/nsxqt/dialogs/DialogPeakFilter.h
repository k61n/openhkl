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
    const nsx::PeakSet& filteredPeaks() const;

public slots:
    virtual void accept() override;

private:
    nsx::PeakSet _peaks;
    Ui::PeakFilterDialog* _ui;
};
