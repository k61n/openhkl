#pragma once

#include <QDialog>
#include <set>

#include <nsxlib/CrystalTypes.h>

namespace Ui {
class DialogPredictPeaks;
}

class DialogPredictPeaks : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPredictPeaks(const std::set<nsx::sptrUnitCell>& cells, QWidget *parent = 0);
    ~DialogPredictPeaks();

    double dMin();
    double dMax();
    double Isigma();
    double sigmaD();
    double sigmaM();
    double radius();
    double nframes();

    int minimumNeighbors();

    nsx::sptrUnitCell cell();
    

private:
    Ui::DialogPredictPeaks *ui;
    std::set<nsx::sptrUnitCell> _cells;
};
