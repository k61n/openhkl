//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_experiment/properties/SampleShapeProperties.cpp
//! @brief     Implements class SampleShapeProperties
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/properties/SampleShapeProperties.h"
#include "base/utils/Units.h"
#include "core/experiment/Experiment.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QVBoxLayout>

SampleShapeProperties::SampleShapeProperties()
{
    QVBoxLayout* overallLayout = new QVBoxLayout(this);
    // SampleProperty
    QGroupBox* sampleProperty = new QGroupBox("Goniometer", this);
    QVBoxLayout* box = new QVBoxLayout(sampleProperty);
    sampleGoniometer = new QTableWidget(this);
    box->addWidget(sampleGoniometer);
    // ShapeProperty
    QGroupBox* shape = new QGroupBox("Shape", this);
    QFormLayout* form = new QFormLayout(shape);

    _movie = new QLineEdit();
    _movie->setReadOnly(true);
    _volume = new QLineEdit();
    _volume->setReadOnly(true);
    _faces = new QLineEdit();
    _faces->setReadOnly(true);
    _edges = new QLineEdit();
    _edges->setReadOnly(true);
    _vertices = new QLineEdit();
    _vertices->setReadOnly(true);

    // form->addRow(loadMovieButton, movie);
    form->addRow("Volume", _volume);
    form->addRow("Faces", _faces);
    form->addRow("Edges", _edges);
    form->addRow("Vertices", _vertices);

    // move together
    overallLayout->addWidget(new QLabel("Sample"));
    overallLayout->addWidget(sampleProperty);
    overallLayout->addWidget(shape);
}

void SampleShapeProperties::refreshInput()
{
    if (gSession->currentProjectNum() >= 0) {
        // SampleProperty
        const nsx::Sample& sample =
            gSession->currentProject()->experiment()->getDiffractometer()->sample();
        const nsx::Gonio& sample_gonio = sample.gonio();
        size_t n_sample_gonio_axes = sample_gonio.nAxes();

        sampleGoniometer->setEditTriggers(QAbstractItemView::NoEditTriggers);
        sampleGoniometer->setRowCount(n_sample_gonio_axes);
        sampleGoniometer->setColumnCount(2);

        auto* tablewidgetitem = new QTableWidgetItem();
        tablewidgetitem->setText("Name");
        sampleGoniometer->setHorizontalHeaderItem(0, tablewidgetitem);

        auto* tablewidgetitem1 = new QTableWidgetItem();
        tablewidgetitem1->setText("Type");
        sampleGoniometer->setHorizontalHeaderItem(1, tablewidgetitem1);
        sampleGoniometer->horizontalHeader()->setStretchLastSection(true);
        sampleGoniometer->verticalHeader()->setVisible(false);

        for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
            const nsx::Axis& axis = sample_gonio.axis(i);

            std::ostringstream os;
            os << axis;

            QTableWidgetItem* item0 = new QTableWidgetItem();
            item0->setData(Qt::DisplayRole, QString(axis.name().c_str()));
            item0->setBackground(axis.physical() ? QColor("#FFDDDD") : QColor("#DDFFDD"));
            sampleGoniometer->setItem(i, 0, item0);

            sampleGoniometer->setItem(i, 1, new QTableWidgetItem(QString(os.str().c_str())));
        }

        // Shape
        const nsx::ConvexHull& hull = sample.shape();
        _volume->setText(QString::number(hull.volume() / nsx::mm3) + " mm^3");
        _faces->setText(QString::number(hull.normals().size()));
        _vertices->setText(QString::number(hull.vertices().size()));
        //_edges->setText(QString::number(hull.edges().size()));
    } else {
        // SampleProperty
        sampleGoniometer->removeColumn(1);
        sampleGoniometer->removeColumn(0);
        // Shape
        _volume->setText("");
        _faces->setText("");
        _edges->setText("");
        _vertices->setText("");
    }
}
