
#ifndef PEAKFINDER_H
#define PEAKFINDER_H

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/qcr/widgets/controls.h"
#include <QTableWidget>
#include <QGraphicsView>
#include <QDialogButtonBox>

class PeakFinder : public QcrFrame {
public:
    PeakFinder();
    void updateConvolutionParameters();
    void run();
    std::map<std::string, double> convolutionParameters();

private:
    QcrSpinBox* threshold;
    QcrDoubleSpinBox* mergingScale;
    QcrSpinBox* minSize;
    QcrSpinBox* maxSize;
    QcrSpinBox* maxWidth;
    QComboBox* convolutionKernel;
    QTableWidget* convolutionParams;
    QComboBox* data;
    QcrSpinBox* frame;
    QcrCheckBox* applyThreshold;
    QcrDoubleSpinBox* peakArea;
    QcrDoubleSpinBox* backgroundLowerLimit;
    QcrDoubleSpinBox* backgroundUpperLimit;
    QGraphicsView* preview;
    QDialogButtonBox* buttons;
    QcrTabWidget* tab;

    void breakUp();
private slots:
    void doActions(QAbstractButton*);
};

#endif //PEAKFINDER_H
