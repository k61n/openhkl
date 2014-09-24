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
    void setPeak(SX::Crystal::Peak3D *peak);
    void keyPressEvent(QKeyEvent* event);
    ~PeakPlotter();
public slots:
    void mousePress();
    void mouseWheel();
    void update();
private:
    // Copy the current view to the clipboard.
    void copyViewToClipboard();
    Ui::PeakPlotter *ui;
    SX::Crystal::Peak3D *_current;
};

#endif // PEAKPLOTTER_H
