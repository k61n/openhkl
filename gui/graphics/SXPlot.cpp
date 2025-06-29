//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/SXPlot.cpp
//! @brief     Implements class SXPlot
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics/SXPlot.h"

#include <QApplication>
#include <QFileInfo>
#include <QKeyEvent>
#include <QPainter>
#include <QString>
#include <fstream>


SXPlot::SXPlot(QWidget* parent) : QCustomPlot(parent)
{
    _zoom_box = new QCPItemRect(this);
    _zoom_box->setVisible(false);

    legend->setSelectableParts(QCPLegend::spItems);
    legend->setVisible(false); // legend is buggy, so better turn it off

    setInteractions(QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);

    // Enable right button click to export ASCII data
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QCustomPlot::customContextMenuRequested, this, &SXPlot::setmenuRequested);
}

void SXPlot::addErrorBars(QCPGraph* graph, QVector<double>& error)
{
    errorBars = new QCPErrorBars{graph->keyAxis(), graph->valueAxis()};
    errorBars->setErrorType(QCPErrorBars::ErrorType::etValueError);
    errorBars->setDataPlottable(graph);
    errorBars->setData(error);
}

void SXPlot::update(PlottableItem* item)
{
    Q_UNUSED(item)
}

std::string SXPlot::getType() const
{
    return "";
}

void SXPlot::copyViewToClipboard()
{
    // Create the image with the exact size of the shrunk scene
    QImage image(this->rect().size(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    this->render(&painter);
    painter.end();
    QApplication::clipboard()->setImage(image, QClipboard::Clipboard);
}

void SXPlot::keyPressEvent(QKeyEvent* event)
{
    if (event->matches(QKeySequence::Copy))
        copyViewToClipboard();
}

void SXPlot::mousePressEvent(QMouseEvent* mouse_event)
{
    if (mouse_event->button() == Qt::LeftButton) {
        _zoom_rect_origin = mouse_event->pos();

        _zoom_box->setBrush(QBrush(QColor(160, 160, 164, 50)));
        _zoom_box->topLeft->setCoords(
            xAxis->pixelToCoord(mouse_event->x()), yAxis->pixelToCoord(mouse_event->y()));
        _zoom_box->bottomRight->setCoords(
            xAxis->pixelToCoord(mouse_event->x()), yAxis->pixelToCoord(mouse_event->y()));
        _zoom_box->setVisible(true);
        replot();
    }
    QCustomPlot::mousePress(mouse_event);
}

void SXPlot::mouseMoveEvent(QMouseEvent* mouse_event)
{
    if (mouse_event->button() == Qt::LeftButton || _zoom_box->visible()) {
        _zoom_box->bottomRight->setCoords(
            xAxis->pixelToCoord(mouse_event->x()), yAxis->pixelToCoord(mouse_event->y()));
        replot();
    }
    QCustomPlot::mouseMove(mouse_event);
}

void SXPlot::mouseReleaseEvent(QMouseEvent* mouse_event)
{
    if (mouse_event->button() == Qt::LeftButton || _zoom_box->visible()) {
        _zoom_box->setVisible(false);
        double x1 = xAxis->pixelToCoord(_zoom_rect_origin.x());
        double x2 = xAxis->pixelToCoord(mouse_event->pos().x());
        double y1 = yAxis->pixelToCoord(_zoom_rect_origin.y());
        double y2 = yAxis->pixelToCoord(mouse_event->pos().y());
        zoom(x1, x2, y1, y2);
        emit signalXRange(x1 < x2 ? x1 : x2, x1 < x2 ? x2 : x1);
        emit signalYRange(y1 < y2 ? y1 : y2, y1 < y2 ? y2 : y1);
    }
    QCustomPlot::mouseRelease(mouse_event);
}

void SXPlot::mouseWheelEvent(QWheelEvent* wheel_event)
{
    QRect current_range = axisRect()->rect();

    std::vector<double> edges{
        xAxis->pixelToCoord(current_range.x()),
        xAxis->pixelToCoord(current_range.x() + current_range.width()),
        yAxis->pixelToCoord(current_range.y()),
        yAxis->pixelToCoord(current_range.y() + current_range.height())};

    std::vector<double> mouse_pos{
        xAxis->pixelToCoord(wheel_event->position().x()),
        yAxis->pixelToCoord(wheel_event->position().y())};

    double factor;
    if (wheel_event->angleDelta().y() < 0)
        factor = 1.1;
    else
        factor = 0.9;

    std::vector<double> new_edges{
        mouse_pos[0] - (mouse_pos[0] - edges[0]) * factor,
        mouse_pos[0] - (mouse_pos[0] - edges[1]) * factor,
        mouse_pos[1] - (mouse_pos[1] - edges[2]) * factor,
        mouse_pos[1] - (mouse_pos[1] - edges[3]) * factor};

    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        xAxis->setRange(new_edges[0], new_edges[1]);
    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        yAxis->setRange(new_edges[2], new_edges[3]);
    else {
        xAxis->setRange(new_edges[0], new_edges[1]);
        yAxis->setRange(new_edges[2], new_edges[3]);
    }
    replot();
    QCustomPlot::mouseWheel(wheel_event);
}

void SXPlot::zoom(double x_init, double x_final, double y_init, double y_final)
{
    xAxis->setRange(x_init, x_final);
    yAxis->setRange(y_init, y_final);
    replot();
}

void SXPlot::resetZoom()
{
    // crashed in for loop below -> there have been less plots as graphCount() suggests
    // wrong method ?! Seems to work with graphCount well
    // int plot_count = plottableCount();
    int plot_count = graphCount();

    double x_max = -1e20;
    double x_min = 1e20;
    double y_max = -1e20;
    double y_min = 1e20;

    std::vector<double> values_x, values_y;
    for (int i = 0; i < plot_count; ++i) { // crash -> plot_count does not have the right size ???
        QCPGraph* graph_item = graph(i);
        values_x.clear();
        values_y.clear();

        for (QVector<QCPGraphData>::iterator it = graph_item->data()->begin();
             it != graph_item->data()->end(); ++it) {
            values_x.push_back(it->key);
            values_y.push_back(it->value);
        }

        if (*(std::max_element(values_x.begin(), values_x.end())) > x_max)
            x_max = *(std::max_element(values_x.begin(), values_x.end()));
        if (*(std::min_element(values_x.begin(), values_x.end())) < x_min)
            x_min = *(std::min_element(values_x.begin(), values_x.end()));
        if (*(std::max_element(values_y.begin(), values_y.end())) > y_max)
            y_max = *(std::max_element(values_y.begin(), values_y.end()));
        if (*(std::min_element(values_y.begin(), values_y.end())) < y_min)
            y_min = *(std::min_element(values_y.begin(), values_y.end()));
    }
    zoom(x_min, x_max, y_min, y_max);
    emit signalXRange(x_min, x_max);
    emit signalYRange(y_min, y_max);
}

SXPlot::~SXPlot() = default;

void SXPlot::titleDoubleClick(QMouseEvent* event, QCPTextElement* title)
{
    Q_UNUSED(event)
    // Set the plot title by double clicking on it
    bool ok;
    QString newTitle = QInputDialog::getText(
        this, "OpenHKL", "New plot title:", QLineEdit::Normal, title->text(), &ok);
    if (ok) {
        title->setText(newTitle);
        replot();
    }
}

void SXPlot::legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item)
{
    // Rename a graph by double clicking on its legend item
    Q_UNUSED(legend)
    if (item) // only react if item was clicked (user could have clicked on border
              // padding of legend where there is no item, then item is 0)
    {
        QCPPlottableLegendItem* plItem = qobject_cast<QCPPlottableLegendItem*>(item);
        bool ok;
        QString newName = QInputDialog::getText(
            this, "OpenHKL", "New legend:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
        if (ok) {
            plItem->plottable()->setName(newName);
            replot();
        }
    }
}

void SXPlot::exportToAscii(QCPErrorBars* errorBars, const char lim)
{
    int ngraphs = this->graphCount();

    if (!ngraphs)
        return;

    int npoints = graph(0)->data()->size();
    if (!npoints)
        return;

    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Name CSV file to export"), QString(qgetenv("HOME")), tr("Data file (*.csv)"),
        nullptr, QFileDialog::DontUseNativeDialog);

    std::ofstream file;
    file.open(fileName.toStdString().c_str(), std::ios::out);
    if (!file.is_open())
        QMessageBox::critical(this, tr("OpenHKL"), tr("Problem opening file"));

    for (unsigned ind = 0; ind < npoints; ++ind) {
        file << graph(0)->data()->at(ind)->key << lim;
        for (unsigned ngraph = 0; ngraph < ngraphs; ++ngraph) {
            file << graph(ngraph)->data()->at(ind)->value;
            if (errorBars != NULL) {
                double error = 0.5
                    * (errorBars->data()->at(ind).errorPlus
                       + errorBars->data()->at(ind).errorMinus);
                file << lim << error;
            }
        }
        file << std::endl;
    }
    file.close();
}

void SXPlot::setmenuRequested(QPoint pos)
{
    // Add menu to export the graphs to ASCII if graphs are present
    if (this->graphCount()) {
        QMenu* menu = new QMenu(this);

        QAction* reset_zoom = menu->addAction("Reset zoom");
        menu->popup(mapToGlobal(pos));

        connect(reset_zoom, &QAction::triggered, this, &SXPlot::resetZoom);

        menu->addSeparator();

        QAction* copy_clpbrd = menu->addAction("Copy to clipboard");
        menu->popup(mapToGlobal(pos));
        connect(copy_clpbrd, &QAction::triggered, this, [=]() {
            QApplication::clipboard()->setImage(toPixmap().toImage(), QClipboard::Clipboard);
        });

        // saving images to file
        QAction* save_graph = menu->addAction("Save graph");
        menu->popup(mapToGlobal(pos));
        connect(save_graph, &QAction::triggered, this, [=]() {
            QFileInfo fi(QFileDialog::getSaveFileName(
                             this, tr("Save image as"), QString(qgetenv("HOME")), tr("Images (*.png *.jpg)"),
                             nullptr, QFileDialog::DontUseNativeDialog));

            if (!fi.absoluteFilePath().isNull()) {
                if (fi.suffix() == "png")
                    if (!savePng(fi.absoluteFilePath(), 1400, 800, 1.0, 100))
                        QMessageBox::information(
                            this, "Saving file failed", "The file couldn't be saved.");
                if (fi.suffix() == "jpg")
                    if (!saveJpg(fi.absoluteFilePath(), 1400, 800, 1.0, 100))
                        QMessageBox::information(
                            this, "Saving file failed", "The file couldn't be saved.");
            }
        });

        QAction* export_ASCII = menu->addAction("Export to CSV");
        menu->popup(mapToGlobal(pos));

        connect(export_ASCII, &QAction::triggered, this, [=]() {
            QCPErrorBars* errorBars = new QCPErrorBars(xAxis, yAxis);
            errorBars->removeFromLegend();
            errorBars->setAntialiased(false);
            errorBars->setDataPlottable(graph(3));
            errorBars->setPen(QPen(QColor(180, 180, 180)));
            // Plot->graph(3)->setName("Measurement");

            exportToAscii(errorBars);
        });
    }
}
