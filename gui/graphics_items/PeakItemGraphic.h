//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_items/PeakItemGraphic.h
//! @brief     Defines class PeakItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_GRAPHICS_ITEMS_PEAKITEMGRAPHIC_H
#define NSX_GUI_GRAPHICS_ITEMS_PEAKITEMGRAPHIC_H

#include "core/peak/Peak3D.h"
#include "gui/graphics_items/PlottableItem.h"
#include "gui/widgets/PeakViewWidget.h"

#include <Eigen/Dense>
#include <QColor>

class Peak3D;

//! Plottable graphics item that represents a peak in the detector image
class PeakItemGraphic : public PlottableItem {
 public:
    PeakItemGraphic(ohkl::Peak3D* peak);
    ~PeakItemGraphic() = default;

    //! Redraw all the elements of the item
    void redraw();
    //! Set the center of the element
    void setCenter(int frame);

    void plot(SXPlot* plot) override;

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    //! Returns the type of plot related to the item
    std::string getPlotType() const override;
    //! return the actual peak pointer
    ohkl::Peak3D* peak() const;
    //! Show the labels
    void showLabel(bool flag);
    //! Show the peak area
    void showArea(bool flag);
    //! Change the peak size
    void setSize(int size);
    //! Change the peak color
    void setColor(QColor color);
    //! Change the peak center color
    void setCenterColor(QColor color);

    //! Init properties from a set of controls on \a peakViewWidget
    void initFromPeakViewWidget(const PeakViewWidget::Set& set);

 private:
    //! Pointer to the Peak3D object
    ohkl::Peak3D* _peak;
    //! Flag to show the label
    bool _show_label;
    //! Flag to show the area
    bool _show_center;
    //! The area item
    QGraphicsEllipseItem* _center_gi;
    //! Lower boundaries
    Eigen::Vector3d _lower;
    //! Upper boundaries
    Eigen::Vector3d _upper;
    //! Size
    Eigen::Vector2d _size;
    //! Peak centre colour
    QColor _center_color;
};

#endif // NSX_GUI_GRAPHICS_ITEMS_PEAKITEMGRAPHIC_H
