//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/frames/InstrumentStatesFrame.h
//! @brief     Defines class InstrumentStatesFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_FRAMES_INSTRUMENTSTATESFRAME_H
#define OHKL_GUI_FRAMES_INSTRUMENTSTATESFRAME_H

#include <QDoubleSpinBox>
#include <QFrame>
#include <QLabel>
#include <QListWidget>
#include <QSpinBox>

//! Frame which shows the instrument states
class InstrumentStatesFrame : public QFrame {
 public:
    InstrumentStatesFrame();

 private:
    void selectedDataChanged(int selectedData);
    void selectedFrameChanged(int selectedFrame);

    QSpinBox* frameIndex;
    QSlider* frameSlider;
    QListWidget* data;
    QLabel* refinedLabel;

    QDoubleSpinBox* samplePosX;
    QDoubleSpinBox* samplePosY;
    QDoubleSpinBox* samplePosZ;

    QDoubleSpinBox* sampleOri_00;
    QDoubleSpinBox* sampleOri_01;
    QDoubleSpinBox* sampleOri_02;
    QDoubleSpinBox* sampleOri_10;
    QDoubleSpinBox* sampleOri_11;
    QDoubleSpinBox* sampleOri_12;
    QDoubleSpinBox* sampleOri_20;
    QDoubleSpinBox* sampleOri_21;
    QDoubleSpinBox* sampleOri_22;

    QDoubleSpinBox* sampleOff_00;
    QDoubleSpinBox* sampleOff_01;
    QDoubleSpinBox* sampleOff_02;
    QDoubleSpinBox* sampleOff_10;
    QDoubleSpinBox* sampleOff_11;
    QDoubleSpinBox* sampleOff_12;
    QDoubleSpinBox* sampleOff_20;
    QDoubleSpinBox* sampleOff_21;
    QDoubleSpinBox* sampleOff_22;

    QDoubleSpinBox* detectorOri_00;
    QDoubleSpinBox* detectorOri_01;
    QDoubleSpinBox* detectorOri_02;
    QDoubleSpinBox* detectorOri_10;
    QDoubleSpinBox* detectorOri_11;
    QDoubleSpinBox* detectorOri_12;
    QDoubleSpinBox* detectorOri_20;
    QDoubleSpinBox* detectorOri_21;
    QDoubleSpinBox* detectorOri_22;

    QDoubleSpinBox* beamX;
    QDoubleSpinBox* beamY;
    QDoubleSpinBox* beamZ;
};

#endif // OHKL_GUI_FRAMES_INSTRUMENTSTATESFRAME_H
