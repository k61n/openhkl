#ifndef NSXQT_DIALOGCALCULATEDPEAKS_H
#define NSXQT_DIALOGCALCULATEDPEAKS_H

#include <QDialog>
#include <set>

#include <nsxlib/CrystalTypes.h>

namespace Ui {
class DialogCalculatedPeaks;
}

class DialogCalculatedPeaks : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCalculatedPeaks(const std::set<nsx::sptrUnitCell>& cells, QWidget *parent = 0);
    ~DialogCalculatedPeaks();

    double dMin();
    double dMax();
    double peakScale();
    double bkgScale();
    double Isigma();
    double sigmaD();
    double sigmaM();

    int minimumNeighbors();

    nsx::sptrUnitCell cell();
    

private:
    Ui::DialogCalculatedPeaks *ui;
    std::set<nsx::sptrUnitCell> _cells;
};

#endif // NSXQT_DIALOGCALCULATEDPEAKS_H
