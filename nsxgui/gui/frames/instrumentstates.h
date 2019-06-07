
#ifndef INSTRUMENTSTATES_H
#define INSTRUMENTSTATES_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <QListWidget>
#include <QSlider>

class InstrumentStates : public QcrFrame {
public:
    InstrumentStates();

private:
    QcrSpinBox* frameIndex;
    QSlider* frameSlider;
    QListWidget* data;
    QcrDoubleSpinBox* samplePosX;
    QcrDoubleSpinBox* samplePosY;
    QcrDoubleSpinBox* samplePosZ;

    QcrDoubleSpinBox* sampleOri_00;
    QcrDoubleSpinBox* sampleOri_01;
    QcrDoubleSpinBox* sampleOri_02;
    QcrDoubleSpinBox* sampleOri_10;
    QcrDoubleSpinBox* sampleOri_11;
    QcrDoubleSpinBox* sampleOri_12;
    QcrDoubleSpinBox* sampleOri_20;
    QcrDoubleSpinBox* sampleOri_21;
    QcrDoubleSpinBox* sampleOri_22;

    QcrDoubleSpinBox* sampleOff_00;
    QcrDoubleSpinBox* sampleOff_01;
    QcrDoubleSpinBox* sampleOff_02;
    QcrDoubleSpinBox* sampleOff_10;
    QcrDoubleSpinBox* sampleOff_11;
    QcrDoubleSpinBox* sampleOff_12;
    QcrDoubleSpinBox* sampleOff_20;
    QcrDoubleSpinBox* sampleOff_21;
    QcrDoubleSpinBox* sampleOff_22;

    QcrDoubleSpinBox* detectorOri_00;
    QcrDoubleSpinBox* detectorOri_01;
    QcrDoubleSpinBox* detectorOri_02;
    QcrDoubleSpinBox* detectorOri_10;
    QcrDoubleSpinBox* detectorOri_11;
    QcrDoubleSpinBox* detectorOri_12;
    QcrDoubleSpinBox* detectorOri_20;
    QcrDoubleSpinBox* detectorOri_21;
    QcrDoubleSpinBox* detectorOri_22;

    QcrDoubleSpinBox* beamX;
    QcrDoubleSpinBox* beamY;
    QcrDoubleSpinBox* beamZ;
};

#endif // INSTRUMENTSTATES_H
