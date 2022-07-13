//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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

#ifndef OHKL_GUI_FRAMES_GLOBALOFFSETSFRAME_H
#define OHKL_GUI_FRAMES_GLOBALOFFSETSFRAME_H

#include "gui/graphics/SXPlot.h"

#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QListWidget>
#include <QSpinBox>
#include <QTableWidget>

enum class offsetMode { DETECTOR, SAMPLE };

//! Frame which shows the global offsets of the detector or sample
class GlobalOffsetsFrame : public QFrame {
 public:
    GlobalOffsetsFrame(offsetMode mode);

 private:
    void layout();
    void fit();
    void actionClicked(QAbstractButton* button);

    QSpinBox* iterations;
    QDoubleSpinBox* tolerance;
    QListWidget* selectedData;
    QTableWidget* offsets;
    QDialogButtonBox* buttons;
    offsetMode mode_;
    SXPlot* plot;
};

#endif // OHKL_GUI_FRAMES_GLOBALOFFSETSFRAME_H
