
//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utilities/PeakViewWidget.cpp
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

#include <QLabel>

PeakViewWidget::PeakViewWidget(std::string type1, std::string type2)
{
    setSizePolicies();

    QLabel* show_label = new QLabel("Show:");
    show_label->setAlignment(Qt::AlignRight);

    QLabel* type1_label = new QLabel(QString::fromStdString(type1));
    type1_label->setAlignment(Qt::AlignLeft);

    QLabel* type1_size_label = new QLabel("Size:");
    type1_size_label->setAlignment(Qt::AlignRight);

    QLabel* type1_color_label = new QLabel("Colour:");
    type1_color_label->setAlignment(Qt::AlignRight);

    QLabel* type2_label = new QLabel(QString::fromStdString(type2));
    type2_label->setAlignment(Qt::AlignLeft);

    QLabel* type2_size_label = new QLabel("Size:");
    type2_size_label->setAlignment(Qt::AlignRight);

    QLabel* type2_color_label = new QLabel("Colour:");
    type2_color_label->setAlignment(Qt::AlignRight);

    _draw_peaks_1 = new QCheckBox("Peaks");
    _draw_peaks_1->setCheckState(Qt::CheckState::Checked);

    _draw_bbox_1 = new QCheckBox("Boxes");
    _draw_bbox_1->setCheckState(Qt::CheckState::Unchecked);

    _width_peaks_1 = new QSpinBox();
    _width_peaks_1->setValue(10);

    _width_bbox_1 = new QSpinBox();
    _width_bbox_1->setValue(10);

    _color_peaks_1 = new ColorButton();
    _color_peaks_1->changeColor(Qt::darkGreen);

    _color_bbox_1 = new ColorButton();
    _color_bbox_1->changeColor(Qt::darkGreen);

    _draw_peaks_2 = new QCheckBox("Peaks");
    _draw_peaks_2->setCheckState(Qt::CheckState::Checked);

    _draw_bbox_2 = new QCheckBox("Boxes");
    _draw_bbox_2->setCheckState(Qt::CheckState::Unchecked);

    _width_peaks_2 = new QSpinBox();
    _width_peaks_2->setValue(10);

    _width_bbox_2 = new QSpinBox();
    _width_bbox_2->setValue(10);

    _color_peaks_2 = new ColorButton();
    _color_peaks_2->changeColor(Qt::red);

    _color_bbox_2 = new ColorButton();
    _color_bbox_2->changeColor(Qt::red);

    _draw_peaks_1->setMaximumWidth(1000);
    _draw_peaks_2->setMaximumWidth(1000);
    _draw_bbox_1->setMaximumWidth(1000);
    _draw_bbox_2->setMaximumWidth(1000);
    _width_peaks_1->setMaximumWidth(1000);
    _width_peaks_2->setMaximumWidth(1000);
    _width_bbox_1->setMaximumWidth(1000);
    _width_bbox_2->setMaximumWidth(1000);
    _color_peaks_1->setMaximumWidth(1000);
    _color_peaks_2->setMaximumWidth(1000);
    _color_bbox_1->setMaximumWidth(1000);
    _color_bbox_2->setMaximumWidth(1000);

    _draw_peaks_1->setSizePolicy(*_size_policy_widgets);
    _draw_peaks_2->setSizePolicy(*_size_policy_widgets);
    _draw_bbox_1->setSizePolicy(*_size_policy_widgets);
    _draw_bbox_2->setSizePolicy(*_size_policy_widgets);
    _width_peaks_1->setSizePolicy(*_size_policy_widgets);
    _width_peaks_2->setSizePolicy(*_size_policy_widgets);
    _width_bbox_1->setSizePolicy(*_size_policy_widgets);
    _width_bbox_2->setSizePolicy(*_size_policy_widgets);
    _color_peaks_1->setSizePolicy(*_size_policy_widgets);
    _color_peaks_2->setSizePolicy(*_size_policy_widgets);
    _color_bbox_1->setSizePolicy(*_size_policy_widgets);
    _color_bbox_2->setSizePolicy(*_size_policy_widgets);

    addWidget(type1_label, 0, 0, 1, 2);
    addWidget(show_label, 1, 0, 1, 1);
    addWidget(_draw_peaks_1, 1, 1, 1, 1);
    addWidget(_draw_bbox_1, 1, 2, 1, 1);
    addWidget(type1_size_label, 2, 0, 1, 1);
    addWidget(_width_peaks_1, 2, 1, 1, 1);
    addWidget(_width_bbox_1, 2, 2, 1, 1);
    addWidget(type1_color_label, 3, 0, 1, 1);
    addWidget(_color_peaks_1, 3, 1, 1, 1);
    addWidget(_color_bbox_1, 3, 2, 1, 1);

    addWidget(type2_label, 4, 0, 1, 2);
    addWidget(show_label, 5, 0, 1, 1);
    addWidget(_draw_peaks_2, 5, 1, 1, 1);
    addWidget(_draw_bbox_2, 5, 2, 1, 1);
    addWidget(type2_size_label, 6, 0, 1, 1);
    addWidget(_width_peaks_2, 6, 1, 1, 1);
    addWidget(_width_bbox_2, 6, 2, 1, 1);
    addWidget(type2_color_label, 7, 0, 1, 1);
    addWidget(_color_peaks_2, 7, 1, 1, 1);
    addWidget(_color_bbox_2, 7, 2, 1, 1);
}

void PeakViewWidget::setSizePolicies()
{
    _size_policy_widgets = new QSizePolicy();
    _size_policy_widgets->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_widgets->setVerticalPolicy(QSizePolicy::Fixed);
}

QCheckBox* PeakViewWidget::drawPeaks1()
{
    return _draw_peaks_1;
}

QCheckBox* PeakViewWidget::drawBoxes1()
{
    return _draw_bbox_1;
}

QCheckBox* PeakViewWidget::drawPeaks2()
{
    return _draw_peaks_2;
}

QCheckBox* PeakViewWidget::drawBoxes2()
{
    return _draw_bbox_2;
}

ColorButton* PeakViewWidget::peakColor1()
{
    return _color_peaks_1;
}

ColorButton* PeakViewWidget::boxColor1()
{
    return _color_bbox_1;
}

ColorButton* PeakViewWidget::peakColor2()
{
    return _color_peaks_2;
}

ColorButton* PeakViewWidget::boxColor2()
{
    return _color_bbox_2;
}

QSpinBox* PeakViewWidget::peakSize1()
{
    return _width_peaks_1;
}

QSpinBox* PeakViewWidget::boxSize1()
{
    return _width_bbox_1;
}

QSpinBox* PeakViewWidget::peakSize2()
{
    return _width_peaks_2;
}

QSpinBox* PeakViewWidget::boxSize2()
{
    return _width_bbox_2;
}
