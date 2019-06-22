//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/DetectorPropertyWidget.cpp
//! @brief     Implements class DetectorPropertyWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QDoubleSpinBox>
#include <QSpinBox>

#include "core/detector/Detector.h"
#include "base/geometry/DirectVector.h"
#include "core/gonio/Gonio.h"
#include "core/gonio/RotAxis.h"
#include "core/instrument/Diffractometer.h"
#include "base/utils/Units.h"

#include "apps/models/DetectorItem.h"
#include "apps/tree/DetectorPropertyWidget.h"

#include "ui_DetectorPropertyWidget.h"

DetectorPropertyWidget::DetectorPropertyWidget(DetectorItem* caller, QWidget* parent)
    : QWidget(parent), _ui(new Ui::DetectorPropertyWidget), _detectorItem(caller)
{
    _ui->setupUi(this);

    const auto* detector = _detectorItem->experiment()->diffractometer()->detector();
    const auto& detector_gonio = detector->gonio();
    size_t n_detector_gonio_axes = detector_gonio.nAxes();

    _ui->height->setValue(detector->height());
    _ui->width->setValue(detector->width());
    _ui->columns->setValue(detector->nCols());
    _ui->rows->setValue(detector->nRows());
    _ui->sample_to_detector_distance->setValue(detector->distance());

    _ui->axes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _ui->axes->setRowCount(n_detector_gonio_axes);

    _ui->axes->setColumnCount(2);
    _ui->axes->verticalHeader()->setVisible(false);

    for (size_t i = 0; i < n_detector_gonio_axes; ++i) {
        const auto& axis = detector_gonio.axis(i);

        std::ostringstream os;
        os << axis;

        QTableWidgetItem* item0 = new QTableWidgetItem();
        item0->setData(Qt::DisplayRole, QString(axis.name().c_str()));
        item0->setBackgroundColor(axis.physical() ? QColor("#FFDDDD") : QColor("#DDFFDD"));
        _ui->axes->setItem(i, 0, item0);

        _ui->axes->setItem(i, 1, new QTableWidgetItem(QString(os.str().c_str())));
    }

    connect(
        _ui->sample_to_detector_distance,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        [=](double value) { onSampleToDectorDistanceChanged(value); });
}

DetectorPropertyWidget::~DetectorPropertyWidget()
{
    delete _ui;
}

void DetectorPropertyWidget::onSampleToDectorDistanceChanged(double distance)
{
    auto* detector = _detectorItem->experiment()->diffractometer()->detector();

    detector->setDistance(distance);
}
