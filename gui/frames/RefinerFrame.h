//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/RefinerFrame.h
//! @brief     Defines classes RefinerFrame, RefinerFitWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_FRAMES_REFINERFRAME_H
#define GUI_FRAMES_REFINERFRAME_H

#include "core/algo/Refiner.h"
#include "gui/models/PeaksTable.h"
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QDialogButtonBox>
#include <QListWidget>

//! Frame which shows the settings for the refiner
class RefinerFrame : public QcrFrame {
 public:
    RefinerFrame();

 private:
    void layout();
    void accept();
    void refine();
    void actionClicked(QAbstractButton* button);
    void tabRemoved(int index);

    QcrTabWidget* tabs;
    QcrWidget* settings;
    PeaksTableView* peaks;
    QcrCheckBox* refine_lattice;
    QcrCheckBox* refine_samplePosition;
    QcrCheckBox* refine_detectorPosition;
    QcrCheckBox* refine_sampleOrientation;
    QcrCheckBox* refine_ki;
    QcrSpinBox* numberBatches;
    QDialogButtonBox* buttons;
};

//! Tab of the RefinerFrame which shows the refiner fit after refining
class RefinerFitWidget : public QcrWidget {
 public:
    RefinerFitWidget(const std::map<nsx::sptrDataSet, nsx::Refiner>& refiners);

 private:
    void selectedDataChanged(int selected_data);
    void selectedBatchChanged(int selected_batch);
    void selectedFrameChanged(int selected_frame);
    void layout();

    std::map<nsx::sptrDataSet, nsx::Refiner> _refiners;

    QcrSpinBox* batch;
    QcrDoubleSpinBox* samplePosX;
    QcrDoubleSpinBox* samplePosY;
    QcrDoubleSpinBox* samplePosZ;
    QcrDoubleSpinBox* detectorPosX;
    QcrDoubleSpinBox* detectorPosY;
    QcrDoubleSpinBox* detectorPosZ;
    QcrDoubleSpinBox* niX;
    QcrDoubleSpinBox* niY;
    QcrDoubleSpinBox* niZ;
    QcrDoubleSpinBox* wavelength;
    QcrDoubleSpinBox* wavelengthOffset;
    QcrDoubleSpinBox* sampleOrientation00;
    QcrDoubleSpinBox* sampleOrientation01;
    QcrDoubleSpinBox* sampleOrientation02;
    QcrDoubleSpinBox* sampleOrientation10;
    QcrDoubleSpinBox* sampleOrientation11;
    QcrDoubleSpinBox* sampleOrientation12;
    QcrDoubleSpinBox* sampleOrientation20;
    QcrDoubleSpinBox* sampleOrientation21;
    QcrDoubleSpinBox* sampleOrientation22;
    QcrDoubleSpinBox* detectorOrientation00;
    QcrDoubleSpinBox* detectorOrientation01;
    QcrDoubleSpinBox* detectorOrientation02;
    QcrDoubleSpinBox* detectorOrientation10;
    QcrDoubleSpinBox* detectorOrientation11;
    QcrDoubleSpinBox* detectorOrientation12;
    QcrDoubleSpinBox* detectorOrientation20;
    QcrDoubleSpinBox* detectorOrientation21;
    QcrDoubleSpinBox* detectorOrientation22;
    QcrSpinBox* frame;
    QListWidget* selectedData;
};

#endif // GUI_FRAMES_REFINERFRAME_H
