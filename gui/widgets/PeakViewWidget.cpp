
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

#include <QLabel>

PeakViewWidget::PeakViewWidget(std::string type1, std::string type2)
{
    setSizePolicies();

    QLabel* label_ptr;

    label_ptr = new QLabel(QString::fromStdString(type1));
    label_ptr->setAlignment(Qt::AlignLeft);
    addWidget(label_ptr, 0, 0, 1, 2);

    label_ptr = new QLabel("Show:");
    label_ptr->setAlignment(Qt::AlignRight);
    addWidget(label_ptr, 1, 0, 1, 1);

    label_ptr = new QLabel("Size:");
    label_ptr->setAlignment(Qt::AlignRight);
    addWidget(label_ptr, 2, 0, 1, 1);

    label_ptr = new QLabel("Colour:");
    label_ptr->setAlignment(Qt::AlignRight);
    addWidget(label_ptr, 3, 0, 1, 1);

    label_ptr = new QLabel("Bounding boxes:");
    label_ptr->setAlignment(Qt::AlignLeft);
    addWidget(label_ptr, 4, 0, 1, 3);

    label_ptr = new QLabel("Show:");
    label_ptr->setAlignment(Qt::AlignLeft);
    addWidget(label_ptr, 5, 0, 1, 1);

    label_ptr = new QLabel("Colour:");
    label_ptr->setAlignment(Qt::AlignLeft);
    addWidget(label_ptr, 6, 0, 1, 1);

    label_ptr = new QLabel(QString::fromStdString(type2));
    label_ptr->setAlignment(Qt::AlignLeft);
    addWidget(label_ptr, 7, 0, 1, 2);

    label_ptr = new QLabel("Show:");
    label_ptr->setAlignment(Qt::AlignRight);
    addWidget(label_ptr, 8, 0, 1, 1);

    label_ptr = new QLabel("Size:");
    label_ptr->setAlignment(Qt::AlignRight);
    addWidget(label_ptr, 9, 0, 1, 1);

    label_ptr = new QLabel("Colour:");
    label_ptr->setAlignment(Qt::AlignRight);
    addWidget(label_ptr, 10, 0, 1, 1);

    label_ptr = new QLabel("Bounding boxes:");
    label_ptr->setAlignment(Qt::AlignLeft);
    addWidget(label_ptr, 11, 0, 1, 3);

    label_ptr = new QLabel("Show:");
    label_ptr->setAlignment(Qt::AlignLeft);
    addWidget(label_ptr, 12, 0, 1, 1);

    label_ptr = new QLabel("Colour:");
    label_ptr->setAlignment(Qt::AlignLeft);
    addWidget(label_ptr, 13, 0, 1, 1);

    _draw_peaks_1 = new QCheckBox("Centres");
    _draw_peaks_1->setCheckState(Qt::CheckState::Checked);

    _draw_bbox_1 = new QCheckBox("Peak");
    _draw_bbox_1->setCheckState(Qt::CheckState::Unchecked);

    _draw_bkg_1 = new QCheckBox("Background");
    _draw_bkg_1->setCheckState(Qt::CheckState::Unchecked);

    _width_peaks_1 = new QSpinBox();
    _width_peaks_1->setValue(10);

    _color_peaks_1 = new ColorButton();
    _color_peaks_1->changeColor(Qt::darkGreen);

    _color_bbox_1 = new ColorButton();
    _color_bbox_1->changeColor(Qt::darkGreen);

    _color_bkg_1 = new ColorButton();
    _color_bkg_1->changeColor(Qt::darkGreen);

    _draw_peaks_2 = new QCheckBox("Centres");
    _draw_peaks_2->setCheckState(Qt::CheckState::Checked);

    _draw_bbox_2 = new QCheckBox("Peak");
    _draw_bbox_2->setCheckState(Qt::CheckState::Unchecked);

    _draw_bkg_2 = new QCheckBox("Background");
    _draw_bkg_2->setCheckState(Qt::CheckState::Unchecked);

    _width_peaks_2 = new QSpinBox();
    _width_peaks_2->setValue(10);

    _color_peaks_2 = new ColorButton();
    _color_peaks_2->changeColor(Qt::darkRed);

    _color_bbox_2 = new ColorButton();
    _color_bbox_2->changeColor(Qt::darkRed);

    _color_bkg_2 = new ColorButton();
    _color_bkg_2->changeColor(Qt::darkRed);

    _draw_peaks_1->setMaximumWidth(1000);
    _draw_peaks_2->setMaximumWidth(1000);
    _draw_bbox_1->setMaximumWidth(1000);
    _draw_bbox_2->setMaximumWidth(1000);
    _draw_bkg_1->setMaximumWidth(1000);
    _draw_bkg_2->setMaximumWidth(1000);
    _width_peaks_1->setMaximumWidth(1000);
    _width_peaks_2->setMaximumWidth(1000);
    _color_peaks_1->setMaximumWidth(1000);
    _color_peaks_2->setMaximumWidth(1000);
    _color_bbox_1->setMaximumWidth(1000);
    _color_bbox_2->setMaximumWidth(1000);
    _color_bkg_1->setMaximumWidth(1000);
    _color_bkg_2->setMaximumWidth(1000);

    _draw_peaks_1->setSizePolicy(*_size_policy_widgets);
    _draw_peaks_2->setSizePolicy(*_size_policy_widgets);
    _draw_bbox_1->setSizePolicy(*_size_policy_widgets);
    _draw_bbox_2->setSizePolicy(*_size_policy_widgets);
    _draw_bkg_1->setSizePolicy(*_size_policy_widgets);
    _draw_bkg_2->setSizePolicy(*_size_policy_widgets);
    _width_peaks_1->setSizePolicy(*_size_policy_widgets);
    _width_peaks_2->setSizePolicy(*_size_policy_widgets);
    _color_peaks_1->setSizePolicy(*_size_policy_widgets);
    _color_peaks_2->setSizePolicy(*_size_policy_widgets);
    _color_bbox_1->setSizePolicy(*_size_policy_widgets);
    _color_bbox_2->setSizePolicy(*_size_policy_widgets);
    _color_bkg_1->setSizePolicy(*_size_policy_widgets);
    _color_bkg_2->setSizePolicy(*_size_policy_widgets);

    addWidget(_draw_peaks_1, 1, 1, 1, 1);
    addWidget(_width_peaks_1, 2, 1, 1, 1);
    addWidget(_color_peaks_1, 3, 1, 1, 1);
    addWidget(_draw_bbox_1, 5, 1, 1, 1);
    addWidget(_draw_bkg_1, 5, 2, 1, 1);
    addWidget(_color_bbox_1, 6, 1, 1, 1);
    addWidget(_color_bkg_1, 6, 2, 1, 1);

    addWidget(_draw_peaks_2, 8, 1, 1, 1);
    addWidget(_width_peaks_2, 9, 1, 1, 1);
    addWidget(_color_peaks_2, 10, 1, 1, 1);
    addWidget(_draw_bbox_2, 12, 1, 1, 1);
    addWidget(_draw_bkg_2, 12, 2, 1, 1);
    addWidget(_color_bbox_2, 13, 1, 1, 1);
    addWidget(_color_bkg_2, 13, 2, 1, 1);
}

void PeakViewWidget::setSizePolicies()
{
    _size_policy_widgets = new QSizePolicy();
    _size_policy_widgets->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_widgets->setVerticalPolicy(QSizePolicy::Fixed);
}
