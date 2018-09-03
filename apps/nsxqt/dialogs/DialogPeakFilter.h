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

public:
    DialogPeakFilter(const nsx::PeakList& peaks, QWidget* parent=0);
    virtual ~DialogPeakFilter();
    const nsx::PeakList& badPeaks() const;
    const nsx::PeakList& goodPeaks() const;

public slots:

    void actionClicked(QAbstractButton *button);

    virtual void accept() override;

private slots:

    void slotUnitCellChanged(int index);

private:

    void filterPeaks();

private:
    Ui::DialogPeakFilter* _ui;
    nsx::PeakList _peaks;
    nsx::PeakList _filtered_peaks;
    nsx::PeakList _badPeaks;
    nsx::PeakList _goodPeaks;
};
