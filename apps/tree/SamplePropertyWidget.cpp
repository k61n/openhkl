//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/SamplePropertyWidget.cpp
//! @brief     Implements class SamplePropertyWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <sstream>

#include "core/gonio/Axis.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "base/utils/Units.h"

#include "apps/models/SampleItem.h"
#include "apps/models/UnitCellItem.h"
#include "apps/tree/SamplePropertyWidget.h"

#include "ui_SamplePropertyWidget.h"

SamplePropertyWidget::SamplePropertyWidget(SampleItem* caller, QWidget* parent)
    : QWidget(parent), _ui(new Ui::SamplePropertyWidget), _sampleItem(caller)
{
    _ui->setupUi(this);

    const auto& sample = _sampleItem->experiment()->diffractometer()->sample();
    const auto& sample_gonio = sample.gonio();
    size_t n_sample_gonio_axes = sample_gonio.nAxes();

    _ui->axes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _ui->axes->setRowCount(n_sample_gonio_axes);

    _ui->axes->setColumnCount(2);
    _ui->axes->verticalHeader()->setVisible(false);

    for (size_t i = 0; i < n_sample_gonio_axes; ++i) {

        const auto& axis = sample_gonio.axis(i);

        std::ostringstream os;
        os << axis;

        QTableWidgetItem* item0 = new QTableWidgetItem();
        item0->setData(Qt::DisplayRole, QString(axis.name().c_str()));
        item0->setBackgroundColor(axis.physical() ? QColor("#FFDDDD") : QColor("#DDFFDD"));
        _ui->axes->setItem(i, 0, item0);

        _ui->axes->setItem(i, 1, new QTableWidgetItem(QString(os.str().c_str())));
    }
}

SamplePropertyWidget::~SamplePropertyWidget()
{
    delete _ui;
}
