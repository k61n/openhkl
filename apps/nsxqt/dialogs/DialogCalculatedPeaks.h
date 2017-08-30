#ifndef NSXQT_DIALOGCALCULATEDPEAKS_H
#define NSXQT_DIALOGCALCULATEDPEAKS_H

#include <QDialog>

namespace Ui {
class DialogCalculatedPeaks;
}

class DialogCalculatedPeaks : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCalculatedPeaks(QWidget *parent = 0);
    ~DialogCalculatedPeaks();

    double dMin();
    double dMax();
    double searchRadius();
    double frameRadius();
    double minimumRadius();
    double minimumPeakDuration();
    double peakScale();
    double bkgScale();
    
    int minimumNeighbors();
    

private:
    Ui::DialogCalculatedPeaks *ui;
};

#endif // NSXQT_DIALOGCALCULATEDPEAKS_H
