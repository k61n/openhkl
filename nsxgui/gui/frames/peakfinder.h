//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/frames/peakfinder.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_FRAMES_PEAKFINDER_H
#define NSXGUI_GUI_FRAMES_PEAKFINDER_H

#include "nsxgui/gui/models/peakstable.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QDialogButtonBox>
#include <QGraphicsView>
#include <QTableWidget>

class FoundPeaks : public QcrWidget {
public:
    FoundPeaks(nsx::PeakList, const QString&);
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
    QcrSpinBox* framesBegin;
    QcrSpinBox* framesEnd;
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

#endif // NSXGUI_GUI_FRAMES_PEAKFINDER_H
