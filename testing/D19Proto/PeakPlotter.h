#ifndef PEAKPLOTTER_H
#define PEAKPLOTTER_H

#include <QDialog>
#include "Peak3D.h"

namespace Ui {
class PeakPlotter;
}

class PeakPlotter : public QDialog
{
    Q_OBJECT
    
public:
    explicit PeakPlotter(QWidget *parent = 0);
    void setPeak(const SX::Geometry::Peak3D& peak);
    ~PeakPlotter();
    
private:
    Ui::PeakPlotter *ui;
};

#endif // PEAKPLOTTER_H
