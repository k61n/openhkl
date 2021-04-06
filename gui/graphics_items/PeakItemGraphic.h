//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

#include <Eigen/Dense>
#include <QColor>

class Peak3D;
class PeakViewWidget;

//! Plottable graphics item that represents a peak in the detector image
class PeakItemGraphic : public PlottableItem {
 public:
    PeakItemGraphic(nsx::Peak3D* peak);
    ~PeakItemGraphic() = default;

    //! Redraw all the elements of the item
    void redraw();
    //! Set the center of the element
    void setCenter(int frame);

    void plot(SXPlot* plot) override;

    QRectF boundingRect() const override;
    QRectF boundingRect(const nsx::AABB& aabb) const;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    //! Returns the type of plot related to the item
    std::string getPlotType() const override;
    //! return the actual peak pointer
    nsx::Peak3D* peak() const;
    //! Show the labels
    void showLabel(bool flag);
    //! Show the peak area
    void showArea(bool flag);
    //! Show the bounding box
    void showBox(bool flag);
    //! Show the integration region bounding box
    void showBkg(bool flag);
    //! Change the peak size
    void setSize(int size);
    //! Change the peak color
    void setColor(QColor color);
    //! Change the bounding box color
    void setBoxColor(QColor color);
    void setCenterColor(QColor color);
    //! Change the background box color
    void setBkgColor(QColor color);
    void setBkgOutlineColor(QColor color);

    //! Init properties from controls on \a peakViewWidget. Use set 1, if set1 is true, otherwise
    //! use set 2.
    void initFromPeakViewWidget(PeakViewWidget* peakViewWidget, bool set1);

 private:
    //! Pointer to the Peak3D object
    nsx::Peak3D* _peak;
    //! Flag to show the label
    bool _show_label;
    //! Flag to show the area
    bool _show_center;
    //! Flag to show the bounding box
    bool _show_box;
    //! Flag to show the background bounding box
    bool _show_bkg;
    //! The area item
    QGraphicsEllipseItem* _center_gi;
    //! The bounding box
    QGraphicsRectItem* _bounding_box;
    //! Background bounding box
    QGraphicsRectItem* _bkg_box;
    //! Lower boundaries
    Eigen::Vector3d _lower;
    //! Upper boundaries
    Eigen::Vector3d _upper;
    //! Size
    Eigen::Vector2d _size;
    //! Peak centre colour
    QColor _center_color;
    //! Peak bounding box colour
    QColor _peak_color;
    //! Background bounding box colour
    QColor _bkg_color;
};

#endif // NSX_GUI_GRAPHICS_ITEMS_PEAKITEMGRAPHIC_H
