//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/peakfinder.h
//! @brief     Defines classes FoundPeaks, PeakFinder
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_FRAMES_PEAKFINDER_H
#define GUI_FRAMES_PEAKFINDER_H

#include "gui/graphics/detectorview.h"
#include "gui/models/peakstable.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QDialogButtonBox>
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
    DetectorView* preview;
    QDialogButtonBox* buttons;
    QcrTabWidget* tab;
    QGraphicsPixmapItem* pixmap;

    void refreshPreview();
    void accept();
    void doActions(QAbstractButton*);
};

#endif // GUI_FRAMES_PEAKFINDER_H
