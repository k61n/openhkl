//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/PeakFinderFrame.h
//! @brief     Defines classes FoundPeaks, PeakFinderFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_FRAMES_PEAKFINDERFRAME_H
#define GUI_FRAMES_PEAKFINDERFRAME_H

#include "gui/graphics/DetectorView.h"
#include "gui/models/PeaksTable.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QDialogButtonBox>
#include <QTableWidget>

//! Tab of the PeakFinderFrame which contains the found peaks
class FoundPeaks : public QcrWidget {
 public:
    FoundPeaks(nsx::PeakList, const QString&);
    //! Returns the selected peaks as a PeakList
    nsx::PeakList selectedPeaks();

 private:
    PeaksTableModel* tableModel;
    QcrCheckBox* keepSelectedPeaks;
};

//! Frame which shows the settings to find peaks
class PeakFinderFrame : public QcrWidget {
 public:
    PeakFinderFrame();
    //! Change the convolution parameters
    void updateConvolutionParameters();
    //! Find peaks
    void run();
    std::map<std::string, double> convolutionParameters();
    void refreshData();

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

#endif // GUI_FRAMES_PEAKFINDERFRAME_H
