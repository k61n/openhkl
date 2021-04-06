
//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/widgets/PeakViewWidget.cpp
//! @brief     Implements class PeakViewWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/widgets/PeakViewWidget.h"
#include "gui/utility/ColorButton.h"

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpinBox>

PeakViewWidget::PeakViewWidget(const QString& type1, const QString& type2)
{
    QLabel* label_ptr;

    label_ptr = new QLabel(type1);
    label_ptr->setAlignment(Qt::AlignLeft);
    addWidget(label_ptr, 0, 0, 1, 3);

    label_ptr = new QLabel("Show:");
    label_ptr->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    addWidget(label_ptr, 1, 0, 1, 1);

    label_ptr = new QLabel("Size:");
    label_ptr->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    addWidget(label_ptr, 2, 0, 1, 1);

    label_ptr = new QLabel("Colour:");
    label_ptr->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    addWidget(label_ptr, 3, 0, 1, 1);

    label_ptr = new QLabel("Bounding boxes:");
    label_ptr->setAlignment(Qt::AlignLeft);
    addWidget(label_ptr, 4, 0, 1, 3);

    label_ptr = new QLabel("Show:");
    label_ptr->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    addWidget(label_ptr, 5, 0, 1, 1);

    label_ptr = new QLabel("Colour:");
    label_ptr->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    addWidget(label_ptr, 6, 0, 1, 1);

    label_ptr = new QLabel(type2);
    label_ptr->setAlignment(Qt::AlignLeft);
    addWidget(label_ptr, 7, 0, 1, 3);

    label_ptr = new QLabel("Show:");
    label_ptr->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    addWidget(label_ptr, 8, 0, 1, 1);

    label_ptr = new QLabel("Size:");
    label_ptr->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    addWidget(label_ptr, 9, 0, 1, 1);

    label_ptr = new QLabel("Colour:");
    label_ptr->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    addWidget(label_ptr, 10, 0, 1, 1);

    label_ptr = new QLabel("Bounding boxes:");
    label_ptr->setAlignment(Qt::AlignLeft);
    addWidget(label_ptr, 11, 0, 1, 3);

    label_ptr = new QLabel("Show:");
    label_ptr->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    addWidget(label_ptr, 12, 0, 1, 1);

    label_ptr = new QLabel("Colour:");
    label_ptr->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    addWidget(label_ptr, 13, 0, 1, 1);

    drawPeaks1 = new QCheckBox("Centres");
    drawPeaks1->setCheckState(Qt::CheckState::Checked);

    drawBoxes1 = new QCheckBox("Peak");
    drawBoxes1->setCheckState(Qt::CheckState::Unchecked);

    drawBkg1 = new QCheckBox("Background");
    drawBkg1->setCheckState(Qt::CheckState::Unchecked);

    sizePeaks1 = new QSpinBox();
    sizePeaks1->setValue(10);

    colorPeaks1 = new ColorButton(Qt::darkGreen);
    colorBoxes1 = new ColorButton(Qt::darkGreen);
    colorBkg1 = new ColorButton(Qt::darkGreen);

    drawPeaks2 = new QCheckBox("Centres");
    drawPeaks2->setCheckState(Qt::CheckState::Checked);

    drawBoxes2 = new QCheckBox("Peak");
    drawBoxes2->setCheckState(Qt::CheckState::Unchecked);

    drawBkg2 = new QCheckBox("Background");
    drawBkg2->setCheckState(Qt::CheckState::Unchecked);

    sizePeaks2 = new QSpinBox();
    sizePeaks2->setValue(10);

    colorPeaks2 = new ColorButton(Qt::darkRed);
    colorBoxes2 = new ColorButton(Qt::darkRed);
    colorBkg2 = new ColorButton(Qt::darkRed);

    addWidget(drawPeaks1, 1, 1, 1, 2);
    addWidget(sizePeaks1, 2, 1, 1, 1);
    addWidget(colorPeaks1, 3, 1, 1, 1);
    addWidget(drawBoxes1, 5, 1, 1, 1);
    addWidget(drawBkg1, 5, 2, 1, 1);
    addWidget(colorBoxes1, 6, 1, 1, 1);
    addWidget(colorBkg1, 6, 2, 1, 1);

    addWidget(drawPeaks2, 8, 1, 1, 2);
    addWidget(sizePeaks2, 9, 1, 1, 1);
    addWidget(colorPeaks2, 10, 1, 1, 1);
    addWidget(drawBoxes2, 12, 1, 1, 1);
    addWidget(drawBkg2, 12, 2, 1, 1);
    addWidget(colorBoxes2, 13, 1, 1, 1);
    addWidget(colorBkg2, 13, 2, 1, 1);

    connect(drawPeaks1, &QCheckBox::stateChanged, this, &PeakViewWidget::settingsChanged);
    connect(drawPeaks2, &QCheckBox::stateChanged, this, &PeakViewWidget::settingsChanged);
    connect(drawBoxes1, &QCheckBox::stateChanged, this, &PeakViewWidget::settingsChanged);
    connect(drawBoxes2, &QCheckBox::stateChanged, this, &PeakViewWidget::settingsChanged);
    connect(drawBkg1, &QCheckBox::stateChanged, this, &PeakViewWidget::settingsChanged);
    connect(drawBkg2, &QCheckBox::stateChanged, this, &PeakViewWidget::settingsChanged);
    connect(colorPeaks1, &ColorButton::colorChanged, this, &PeakViewWidget::settingsChanged);
    connect(colorPeaks2, &ColorButton::colorChanged, this, &PeakViewWidget::settingsChanged);
    connect(colorBoxes1, &ColorButton::colorChanged, this, &PeakViewWidget::settingsChanged);
    connect(colorBoxes2, &ColorButton::colorChanged, this, &PeakViewWidget::settingsChanged);
    connect(colorBkg1, &ColorButton::colorChanged, this, &PeakViewWidget::settingsChanged);
    connect(colorBkg2, &ColorButton::colorChanged, this, &PeakViewWidget::settingsChanged);

    connect(
        sizePeaks1, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &PeakViewWidget::settingsChanged);
    connect(
        sizePeaks2, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &PeakViewWidget::settingsChanged);
}
