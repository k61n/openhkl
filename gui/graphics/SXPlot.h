//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/SXPlot.h
//! @brief     Defines class SXPlot
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_SXPLOT_H
#define OHKL_GUI_GRAPHICS_SXPLOT_H

#include "qcustomplot.h"

class PlottableItem;


//! Base class of the plot types of the SubframePlot
class SXPlot : public QCustomPlot {
    Q_OBJECT
 public:
    static SXPlot* create(QWidget* parent);

    explicit SXPlot(QWidget* parent = 0);
    void keyPressEvent(QKeyEvent*);
    virtual ~SXPlot();

    // Error bar handling
    QCPErrorBars* errorBars;
    void addErrorBars(QCPGraph* graph, QVector<double>& error);

    // Getters and setters

    virtual std::string getType() const;

 public:
    //! Manage the mouse move
    void mouseMoveEvent(QMouseEvent* mouse_event);
    //! Manage the mouse press
    void mousePressEvent(QMouseEvent* mouse_event);
    //! Manage the mouse release
    void mouseReleaseEvent(QMouseEvent* mouse_event);
    //! Manage the mouse wheel
    void mouseWheelEvent(QWheelEvent* wheel_event);

    //! Manage the zoom
    void zoom(double x_init, double x_final, double y_init, double y_final);
    //! Automatic zoom to fit all plotables
    void resetZoom();
    //! Update the plot
    virtual void update(PlottableItem* item);

    //! Manage double clicks
    void titleDoubleClick(QMouseEvent* event, QCPTextElement* title);
    //! Double click on the legend
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    //! Context menu
    void setmenuRequested(QPoint);
    //! Export the graphs into column ASCII format
    void exportToAscii(QCPErrorBars* errorBars = NULL, const char lim = ',');

 private:
    void copyViewToClipboard();


 private:
    QPoint _zoom_rect_origin;
    QCPItemRect* _zoom_box;
};

#endif // OHKL_GUI_GRAPHICS_SXPLOT_H
