//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/InstrumentStateWindow.cpp
//! @brief     Implemets class InstrumentStateWindow
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subwindows/InstrumentStateWindow.h"

#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "gui/MainWin.h" // gGui
#include "gui/connect/Sentinel.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/DataComboBox.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

InstrumentStateWindow::InstrumentStateWindow(QWidget* parent) : QDialog(parent)
{
    QWidget* top_widget = new QWidget();

    auto* main_layout = new QVBoxLayout(this);
    auto* top_layout = new QHBoxLayout(top_widget);

    _data_combo = new DataComboBox(this);
    _frame_spin = new QSpinBox();
    _instrument_state_widget = new QWidget(this);

    setStateGridsUp();
    refreshAll();

    top_layout->addWidget(_data_combo);
    top_layout->addItem(new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
    top_layout->addWidget(_frame_spin);

    main_layout->addWidget(top_widget);
    main_layout->addWidget(_instrument_state_widget);

    connect(
        _frame_spin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &InstrumentStateWindow::refreshAll);
    connect(
        gGui->sentinel, &Sentinel::instrumentStatesChanged, this,
        &InstrumentStateWindow::refreshAll);
}

void InstrumentStateWindow::setStateGridsUp()
{
    QHBoxLayout* instrument_state_layout = new QHBoxLayout();
    _instrument_state_widget->setLayout(instrument_state_layout);
    QVector<QLineEdit*> line_edits;

    QWidget* sample_orn_widget = new QWidget();
    QWidget* sample_pos_widget = new QWidget();
    QWidget* detector_pos_widget = new QWidget();
    QWidget* ki_widget = new QWidget();
    QGridLayout* sample_orn_grid = new QGridLayout();
    QGridLayout* sample_pos_grid = new QGridLayout();
    QGridLayout* detector_pos_grid = new QGridLayout();
    QGridLayout* ki_grid = new QGridLayout();
    sample_orn_widget->setLayout(sample_orn_grid);
    sample_pos_widget->setLayout(sample_pos_grid);
    detector_pos_widget->setLayout(detector_pos_grid);
    ki_widget->setLayout(ki_grid);

    for (std::size_t i = 0; i < 3; ++i) {
        QVector<QLineEdit*> vec;
        _sample_orn_elements.push_back(vec);
    }

    sample_pos_grid->addWidget(new QLabel("Sample position"), 0, 0, 1, 1);
    sample_orn_grid->addWidget(new QLabel("Sample orientation"), 0, 0, 1, 3);
    detector_pos_grid->addWidget(new QLabel("Detector position"), 0, 0, 1, 1);
    ki_grid->addWidget(new QLabel("Incident wavevector"), 0, 0, 1, 1);

    QLineEdit* element;
    for (std::size_t i = 0; i < 3; ++i) {
        element = new QLineEdit();
        _sample_pos_elements.push_back(element);
        sample_pos_grid->addWidget(element, i + 1, 0, 1, 1);
        line_edits.push_back(element);
        element = new QLineEdit();
        _detector_pos_elements.push_back(element);
        detector_pos_grid->addWidget(element, i + 1, 0, 1, 1);
        line_edits.push_back(element);
        element = new QLineEdit();
        _ki_elements.push_back(element);
        ki_grid->addWidget(element, i + 1, 0, 1, 1);
        line_edits.push_back(element);
        for (std::size_t j = 0; j < 3; ++j) {
            element = new QLineEdit();
            _sample_orn_elements[i].push_back(element);
            sample_orn_grid->addWidget(element, i + 1, j, 1, 1);
            line_edits.push_back(element);
        }
    }

    for (auto* line_edit : line_edits) {
        line_edit->setReadOnly(true);
        line_edit->setAlignment(Qt::AlignRight);
    }

    instrument_state_layout->addWidget(sample_orn_widget);
    instrument_state_layout->addItem(
        new QSpacerItem(50, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
    instrument_state_layout->addWidget(sample_pos_widget);
    instrument_state_layout->addItem(
        new QSpacerItem(50, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
    instrument_state_layout->addWidget(detector_pos_widget);
    instrument_state_layout->addItem(
        new QSpacerItem(50, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
    instrument_state_layout->addWidget(ki_widget);
}

void InstrumentStateWindow::updateData()
{
    if (!gSession->hasProject())
        return;

    if (!gSession->currentProject()->hasDataSet())
        return;

    _data_combo->refresh();
    _frame_spin->setMaximum(_data_combo->currentData()->nFrames());
}

void InstrumentStateWindow::updateState()
{
    if (!gSession->hasProject())
        return;

    if (!gSession->currentProject()->hasDataSet())
        return;

    const auto data = _data_combo->currentData();
    const auto state = data->instrumentStates()[_frame_spin->value()];
    for (std::size_t i = 0; i < 3; ++i) {
        _sample_pos_elements[i]->setText(QString::number(state.samplePosition[i], 'f', 4));
        _detector_pos_elements[i]->setText(
            QString::number(state.detectorPositionOffset[i], 'f', 4));
        _ki_elements[i]->setText(QString::number(state.ki()[i], 'f', 4));
        for (std::size_t j = 0; j < 3; ++j) {
            _sample_orn_elements[i][j]->setText(
                QString::number(state.sampleOrientationMatrix()(i, j), 'f', 4));
        }
    }
}

void InstrumentStateWindow::refreshAll()
{
    updateData();
    updateState();
}

void InstrumentStateWindow::onFrameChanged(int frame)
{
    _frame_spin->setValue(frame);
}
