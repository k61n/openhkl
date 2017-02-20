#ifndef DIALOGCALCULATEDPEAKS_H
#define DIALOGCALCULATEDPEAKS_H

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

private:
    Ui::DialogCalculatedPeaks *ui;
};

#endif // DIALOGCALCULATEDPEAKS_H
