//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/properties/detectorproperty.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/properties/detectorproperty.h"
#include "gui/models/session.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QTableWidget>
#include <build/core/include/core/Detector.h>

DetectorProperty::DetectorProperty() : QcrWidget {"detectorProperty"}
{
    width = new QcrDoubleSpinBox("adhoc_width", new QcrCell<double>(0.00), 4, 2);
    width->setReadOnly(true);
    height = new QcrDoubleSpinBox("adhoc_height", new QcrCell<double>(0.00), 4, 2);
    height->setReadOnly(true);
    distance = new QcrDoubleSpinBox("adhoc_distance", new QcrCell<double>(0.00), 4, 2);
    distance->setReadOnly(true);
    rows = new QcrSpinBox("adhoc_rows", new QcrCell<int>(0), 4);
    rows->setReadOnly(true);
    columns = new QcrSpinBox("adhoc_columns", new QcrCell<int>(0), 4);
    columns->setReadOnly(true);
    // groupBox Parameters
    QGroupBox* groupBox = new QGroupBox("Parameters", this);
    QGridLayout* gridLayout_2 = new QGridLayout(groupBox);
    QGridLayout* gridLayout = new QGridLayout;
    // Labels
    gridLayout->addWidget(new QLabel("Width"), 0, 0, 1, 1);
    gridLayout->addWidget(new QLabel("Height"), 0, 2, 1, 1);
    gridLayout->addWidget(new QLabel("Rows"), 1, 0, 1, 1);
    gridLayout->addWidget(new QLabel("Columns"), 1, 2, 1, 1);
    gridLayout->addWidget(new QLabel("Distance"), 2, 0, 1, 1);
    // Spin boxes
    gridLayout->addWidget(width, 0, 1, 1, 1);
    gridLayout->addWidget(height, 0, 3, 1, 1);
    gridLayout->addWidget(rows, 1, 1, 1, 1);
    gridLayout->addWidget(columns, 1, 3, 1, 1);
    gridLayout->addWidget(distance, 2, 1, 1, 1);
    gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);
    // groupBox Goniometer
    QGroupBox* group_2 = new QGroupBox("Goniometer", this);
    QVBoxLayout* layout = new QVBoxLayout(group_2);
    axes = new QTableWidget(group_2);
    layout->addWidget(axes);

    QVBoxLayout* vboxlayout = new QVBoxLayout(this);
    vboxlayout->addWidget(groupBox);
    vboxlayout->addWidget(group_2);

    setRemake([this]() { onRemake(); });
    remake();
}

DetectorProperty::~DetectorProperty() {}

void DetectorProperty::onRemake()
{
    if (gSession->selectedExperimentNum() >= 0) {
        nsx::Detector* detector =
            gSession->selectedExperiment()->experiment()->diffractometer()->detector();
        width->setCellValue(detector->width());
        height->setCellValue(detector->height());
        distance->setCellValue(detector->distance());
        rows->setCellValue(detector->nRows());
        columns->setCellValue(detector->nCols());

        const auto& detector_gonio = detector->gonio();
        size_t n_detector_gonio_axes = detector_gonio.nAxes();
        axes->setEditTriggers(QAbstractItemView::NoEditTriggers);
        axes->setRowCount(n_detector_gonio_axes);
        axes->setColumnCount(2);
        QTableWidgetItem* __qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setText("Name");
        axes->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem* __qtablewidgetitem1 = new QTableWidgetItem();
        __qtablewidgetitem1->setText("Type");
        axes->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        axes->horizontalHeader()->setStretchLastSection(true);
        axes->verticalHeader()->setVisible(false);
        for (size_t i = 0; i < n_detector_gonio_axes; ++i) {
            const auto& axis = detector_gonio.axis(i);

            std::ostringstream os;
            os << axis;

            QTableWidgetItem* item0 = new QTableWidgetItem();
            item0->setData(Qt::DisplayRole, QString(axis.name().c_str()));
            item0->setBackgroundColor(axis.physical() ? QColor("#FFDDDD") : QColor("#DDFFDD"));
            axes->setItem(i, 0, item0);

            axes->setItem(i, 1, new QTableWidgetItem(QString(os.str().c_str())));
        }
    } else {
        width->setCellValue(0.00);
        height->setCellValue(0.00);
        distance->setCellValue(0.00);
        rows->setCellValue(0);
        columns->setCellValue(0);
        axes->removeColumn(1);
        axes->removeColumn(0);
    }
}
