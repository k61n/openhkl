//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_experiment/properties/DetectorProperty.cpp
//! @brief     Implements class DetectorProperty
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/properties/DetectorProperty.h"
#include "core/detector/Detector.h"
#include "core/experiment/Experiment.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>

DetectorProperty::DetectorProperty()
{
    QAbstractSpinBox::ButtonSymbols symbols = QAbstractSpinBox::NoButtons;

    _width = new QDoubleSpinBox();
    _height = new QDoubleSpinBox();
    _distance = new QDoubleSpinBox();
    _rows = new QSpinBox();
    _columns = new QSpinBox();

    _width->setMaximum(1000.);
    _width->setDecimals(4);
    _width->setButtonSymbols(symbols);
    _width->setReadOnly(true);

    _height->setMaximum(1000.);
    _height->setDecimals(4);
    _height->setButtonSymbols(symbols);
    _height->setReadOnly(true);

    _distance->setMaximum(1000.);
    _distance->setDecimals(4);
    _distance->setButtonSymbols(symbols);
    _distance->setReadOnly(true);

    _rows->setMaximum(1000);
    _rows->setButtonSymbols(symbols);
    _rows->setReadOnly(true);

    _columns->setMaximum(10000);
    _columns->setButtonSymbols(symbols);
    _columns->setReadOnly(true);

    // groupBox Parameters
    QGroupBox* groupBox = new QGroupBox("Parameters", this);
    QGridLayout* gridLayout = new QGridLayout(groupBox);

    // Labels
    gridLayout->addWidget(new QLabel("Width"), 0, 0, 1, 1);
    gridLayout->addWidget(new QLabel("Height"), 0, 2, 1, 1);
    gridLayout->addWidget(new QLabel("Rows"), 1, 0, 1, 1);
    gridLayout->addWidget(new QLabel("Columns"), 1, 2, 1, 1);
    gridLayout->addWidget(new QLabel("Distance"), 2, 0, 1, 1);

    // Spin boxes
    gridLayout->addWidget(_width, 0, 1, 1, 1);
    gridLayout->addWidget(_height, 0, 3, 1, 1);
    gridLayout->addWidget(_rows, 1, 1, 1, 1);
    gridLayout->addWidget(_columns, 1, 3, 1, 1);
    gridLayout->addWidget(_distance, 2, 1, 1, 1);

    // groupBox Goniometer
    QGroupBox* group_2 = new QGroupBox("Goniometer", this);
    QVBoxLayout* layout = new QVBoxLayout(group_2);
    _axes = new QTableWidget(group_2);
    layout->addWidget(_axes);

    QVBoxLayout* v_box_layout = new QVBoxLayout(this);

    v_box_layout->addWidget(groupBox);
    v_box_layout->addWidget(group_2);
}

DetectorProperty::~DetectorProperty() = default;

void DetectorProperty::refreshInput()
{
    if ((gSession->currentProjectNum() >= 0)
        && gSession->currentProject()->experiment()->getDiffractometer()) {
        nsx::Detector* detector =
            gSession->currentProject()->experiment()->getDiffractometer()->detector();

        _width->setValue(detector->width());
        _height->setValue(detector->height());
        _distance->setValue(detector->distance());
        _rows->setValue(detector->nRows());
        _columns->setValue(detector->nCols());

        const nsx::Gonio& detector_gonio = detector->gonio();
        size_t n_detector_gonio_axes = detector_gonio.nAxes();
        _axes->setEditTriggers(QAbstractItemView::NoEditTriggers);
        _axes->setRowCount(n_detector_gonio_axes);
        _axes->setColumnCount(2);

        auto* tablewidgetitem = new QTableWidgetItem();
        tablewidgetitem->setText("Name");
        _axes->setHorizontalHeaderItem(0, tablewidgetitem);

        auto* tablewidgetitem1 = new QTableWidgetItem();
        tablewidgetitem1->setText("Type");
        _axes->setHorizontalHeaderItem(1, tablewidgetitem1);
        _axes->horizontalHeader()->setStretchLastSection(true);
        _axes->verticalHeader()->setVisible(false);
        for (size_t i = 0; i < n_detector_gonio_axes; ++i) {
            const nsx::Axis& axis = detector_gonio.axis(i);

            std::ostringstream os;
            os << axis;

            QTableWidgetItem* item0 = new QTableWidgetItem();
            item0->setData(Qt::DisplayRole, QString(axis.name().c_str()));
            _axes->setItem(i, 0, item0);

            _axes->setItem(i, 1, new QTableWidgetItem(QString(os.str().c_str())));
        }
    } else {
        _width->setValue(0.00);
        _height->setValue(0.00);
        _distance->setValue(0.00);
        _rows->setValue(0);
        _columns->setValue(0);
        _axes->clear();
    }
}
