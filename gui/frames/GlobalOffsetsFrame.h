//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/GlobalOffsetsFrame.h
//! @brief     Defines classes offsetMode, GlobalOffsetsFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_FRAMES_GLOBALOFFSETSFRAME_H
#define GUI_FRAMES_GLOBALOFFSETSFRAME_H

#include "gui/graphics/SXPlot.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QTableWidget>

enum class offsetMode { DETECTOR, SAMPLE };

//! Frame which shows the global offsets of the detector or sample
class GlobalOffsetsFrame : public QcrFrame {
 public:
    GlobalOffsetsFrame(offsetMode mode);

 private:
    void layout();
    void fit();
    void actionClicked(QAbstractButton* button);

    QcrSpinBox* iterations;
    QcrDoubleSpinBox* tolerance;
    QListWidget* selectedData;
    QTableWidget* offsets;
    QDialogButtonBox* buttons;
    offsetMode mode_;
    SXPlot* plot;
};

#endif // GUI_FRAMES_GLOBALOFFSETSFRAME_H
