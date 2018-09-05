#pragma once

#include <map>
#include <string>

#include <QGroupBox>
#include <QDialog>
#include <QVBoxLayout>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataTypes.h>

class QAbstractButton;

namespace Ui {
class DialogPeakFilter;
}

class DialogPeakFilter : public QDialog {
    Q_OBJECT

public:
    DialogPeakFilter(const nsx::PeakList& peaks, QWidget* parent=0);

    virtual ~DialogPeakFilter();

    const nsx::PeakList& filteredPeaks() const;

public slots:

    virtual void accept() override;

    void slotActionClicked(QAbstractButton *button);

    void slotUnitCellChanged(int index);

private:

    void filterPeaks();

private:
    Ui::DialogPeakFilter* _ui;
    nsx::PeakList _peaks;
    nsx::PeakList _filtered_peaks;
};
