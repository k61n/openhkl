//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/properties/sampleshapeproperties.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "nsxgui/gui/properties/sampleshapeproperties.h"
#include "nsxgui/gui/models/session.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QVBoxLayout>
#include "core/utils/Units.h"

SampleShapeProperties::SampleShapeProperties() : QcrWidget {"sampleShapeProperties"}
{
    QVBoxLayout* overallLayout = new QVBoxLayout(this);
    // SampleProperty
    QGroupBox* sampleProperty = new QGroupBox("Goniometer", this);
    QVBoxLayout* box = new QVBoxLayout(sampleProperty);
    sampleGoniometer = new QTableWidget(this);
    box->addWidget(sampleGoniometer);
    // ShapeProperty
    QGridLayout* grid = new QGridLayout();
    loadMovieButton = new QcrTextTriggerButton("adhoc_movieButton", "Load crystal movie");
    movie = new QcrLineEdit("adhoc_movie", "");
    movie->setReadOnly(true);
    volume = new QcrLineEdit("adhoc_volume", "");
    volume->setReadOnly(true);
    faces = new QcrLineEdit("adhoc_faces", "");
    faces->setReadOnly(true);
    edges = new QcrLineEdit("adhoc_edges", "");
    edges->setReadOnly(true);
    vertices = new QcrLineEdit("adhoc_vertices", "");
    vertices->setReadOnly(true);
    grid->addWidget(loadMovieButton, 0, 0, 1, 1);
    grid->addWidget(movie, 0, 1, 1, 1);
    grid->addWidget(volume, 1, 1, 1, 1);
    grid->addWidget(faces, 2, 1, 1, 1);
    grid->addWidget(edges, 3, 1, 1, 1);
    grid->addWidget(vertices, 4, 1, 1, 1);
    grid->addWidget(new QLabel("Volume"), 1, 0, 1, 1);
    grid->addWidget(new QLabel("Faces"), 2, 0, 1, 1);
    grid->addWidget(new QLabel("Edges"), 3, 0, 1, 1);
    grid->addWidget(new QLabel("Vertices"), 4, 0, 1, 1);
    // move together
    overallLayout->addWidget(new QLabel("Sample"));
    overallLayout->addWidget(sampleProperty);
    overallLayout->addWidget(new QLabel("Shape"));
    overallLayout->addLayout(grid);

    setRemake([this]() { onRemake(); });
}

void SampleShapeProperties::onRemake()
{
    if (gSession->selectedExperimentNum() >= 0) {
        // SampleProperty
        const auto& sample =
            gSession->selectedExperiment()->experiment()->diffractometer()->sample();
        const auto& sample_gonio = sample.gonio();
        size_t n_sample_gonio_axes = sample_gonio.nAxes();

        sampleGoniometer->setEditTriggers(QAbstractItemView::NoEditTriggers);
        sampleGoniometer->setRowCount(n_sample_gonio_axes);

        sampleGoniometer->setColumnCount(2);
        QTableWidgetItem* __qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setText("Name");
        sampleGoniometer->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem* __qtablewidgetitem1 = new QTableWidgetItem();
        __qtablewidgetitem1->setText("Type");
        sampleGoniometer->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        sampleGoniometer->horizontalHeader()->setStretchLastSection(true);
        sampleGoniometer->verticalHeader()->setVisible(false);

        for (size_t i = 0; i < n_sample_gonio_axes; ++i) {

            const auto& axis = sample_gonio.axis(i);

            std::ostringstream os;
            os << axis;

            QTableWidgetItem* item0 = new QTableWidgetItem();
            item0->setData(Qt::DisplayRole, QString(axis.name().c_str()));
            item0->setBackgroundColor(axis.physical() ? QColor("#FFDDDD") : QColor("#DDFFDD"));
            sampleGoniometer->setItem(i, 0, item0);

            sampleGoniometer->setItem(i, 1, new QTableWidgetItem(QString(os.str().c_str())));
        }

        // Shape
        const auto& hull = sample.shape();

        volume->setCellValue(QString::number(hull.volume() / nsx::mm3) + " mm^3");
        faces->setCellValue(QString::number(hull.nFaces()));
        edges->setCellValue(QString::number(hull.nEdges()));
        vertices->setCellValue(QString::number(hull.nVertices()));
    } else {
        // SampleProperty
        sampleGoniometer->removeColumn(1);
        sampleGoniometer->removeColumn(0);
        // Shape
        volume->setCellValue("");
        faces->setCellValue("");
        edges->setCellValue("");
        vertices->setCellValue("");
    }
}
