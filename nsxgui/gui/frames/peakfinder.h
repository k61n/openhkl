
#ifndef PEAKFINDER_H
#define PEAKFINDER_H

#include <QCR/widgets/views.h>
#include <QCR/widgets/controls.h>
#include "nsxgui/gui/models/peakstable.h"
#include <QTableWidget>
#include <QGraphicsView>
#include <QDialogButtonBox>

class FoundPeaks : public QcrWidget {
public:
    FoundPeaks(nsx::PeakList);
    nsx::PeakList selectedPeaks();
private:
    PeaksTableModel* tableModel;
    QcrCheckBox* keepSelectedPeaks;
};

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
    void accept();
private slots:
    void doActions(QAbstractButton*);
};

#endif //PEAKFINDER_H
