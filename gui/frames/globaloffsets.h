//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/globaloffsets.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef GUI_FRAMES_GLOBALOFFSETS_H
#define GUI_FRAMES_GLOBALOFFSETS_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QTableWidget>

enum class offsetMode { DETECTOR, SAMPLE };

class GlobalOffsets : public QcrFrame {
public:
    GlobalOffsets(offsetMode mode);

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
};

#endif // GUI_FRAMES_GLOBALOFFSETS_H
