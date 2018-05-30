#pragma once

#include <QDialog>
#include <QTableView>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/MergedData.h>
#include <nsxlib/SpaceGroup.h>

namespace Ui {
class DialogStatistics;
}

class DialogStatistics : public QDialog
{
    Q_OBJECT

public:
    explicit DialogStatistics(const nsx::PeakList& peaks, const nsx::SpaceGroup& spaceGroup, QWidget *parent = 0);
    ~DialogStatistics();

private slots:

    void update();

    void saveStatistics();

    void saveMergedPeaks();

    void saveUnmergedPeaks();

private:

    void saveToShelX(QTableView* table);

    void saveToFullProf(QTableView* table);

    void updateStatisticsTab();

    void updateMergedPeaksTab();

    void updateUnmergedPeaksTab();

private:

    Ui::DialogStatistics *ui;

    nsx::PeakList _peaks;

    nsx::SpaceGroup _spaceGroup;

    nsx::MergedData _merged_data;
};
