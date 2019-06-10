//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/detectorscene.h
//! @brief     Defines class DetectorScene
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_GRAPHICS_DETECTORSCENE_H
#define GUI_GRAPHICS_DETECTORSCENE_H

#include "core/experiment/CrystalTypes.h"
#include "core/experiment/DataTypes.h"
#include "core/mask/IMask.h"
#include "gui/models/colormap.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QStack>

class PeakItem;
class NSXGraphicsItem;

// For the plotting part, better to have RowMajor matrix to use QImage scanline
// function and optimize cache hit.
typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> rowMatrix;

//! Master Scene containing the pixmap of the detector counts
//! and overlayed graphics items (peaks, data cutters, masks ...)
class DetectorScene : public QGraphicsScene {
    Q_OBJECT
public:
    enum MODE {
        SELECT = 0,
        ZOOM = 1,
        LINE = 2,
        HORIZONTALSLICE = 3,
        VERTICALSLICE = 4,
        MASK = 5,
        ELLIPSE_MASK = 6
    };

    //! Which mode is the cursor diplaying
    enum CURSORMODE { PIXEL = 0, THETA = 1, GAMMA_NU = 2, D_SPACING = 3, MILLER_INDICES = 4 };

    explicit DetectorScene(QObject* parent = 0);

    nsx::sptrDataSet getData() { return _currentData; }
    const rowMatrix& getCurrentFrame() const { return _currentFrame; }
    void setLogarithmic(bool checked) { _logarithmic = checked; }
    void setColorMap(const std::string& name)
    {
        _colormap = std::unique_ptr<ColorMap>(new ColorMap(name));
    }
    //! Load image from current Data and frame
    void loadCurrentImage();
    void clearPeakGraphicsItems();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void wheelEvent(QGraphicsSceneWheelEvent* event);

public slots:
    void resetScene();
    void resetPeakGraphicsItems();
    void setMaxIntensity(int);
    void slotChangeSelectedData(nsx::sptrDataSet peak, int frame);
    void slotChangeSelectedFrame(int frame);
    void slotChangeSelectedPeak(nsx::sptrPeak3D peak);
    void slotChangeEnabledPeak(nsx::sptrPeak3D) { loadCurrentImage(); }
    void slotChangeMaskedPeaks(const nsx::PeakList&) { loadCurrentImage(); }
    void changeInteractionMode(int mode) { _mode = static_cast<MODE>(mode); }
    void changeCursorMode(int mode) { _cursorMode = static_cast<CURSORMODE>(mode); }
    void showPeakLabels(bool flag);
    void showPeakAreas(bool flag);
    void drawIntegrationRegion(bool);
    void updateMasks() { _lastClickedGI = nullptr; }
    int currentFrame() const { return _currentFrameIndex; }

signals:
    //! Signal emitted for all changes of the image
    void dataChanged();
    void signalChangeSelectedData(nsx::sptrDataSet data);
    void signalChangeSelectedFrame(int selected_frame);
    void signalChangeSelectedPeak(nsx::sptrPeak3D peak);

private:
    //! Create the text of the tooltip depending on Scene Mode.
    void createToolTipText(QGraphicsSceneMouseEvent*);
    // find the iterator corresponding to given graphics item
    std::vector<std::pair<QGraphicsItem*, nsx::IMask*>>::iterator findMask(QGraphicsItem* item);

    nsx::sptrDataSet _currentData;
    unsigned long _currentFrameIndex;
    int _currentIntensity;
    rowMatrix _currentFrame;
    CURSORMODE _cursorMode;
    //! Current interaction mode
    MODE _mode;
    //! Point coordinates of the start of zoom region
    QPoint _zoomstart;
    //! Point coordinates of the end of zoom regiom
    QPoint _zoomend;
    //! Graphics Window representing the zoomed area
    QGraphicsRectItem* _zoomrect;
    //! Stack of zoom
    QStack<QRect> _zoomStack;
    bool _itemSelected;
    QGraphicsPixmapItem* _image;
    std::vector<std::pair<QGraphicsItem*, nsx::IMask*>> _masks;
    NSXGraphicsItem* _lastClickedGI;
    bool _logarithmic;
    bool _drawIntegrationRegion;
    std::unique_ptr<ColorMap> _colormap;
    QGraphicsPixmapItem* _integrationRegion;
    QGraphicsRectItem* _selected_peak_gi;
    std::map<nsx::sptrPeak3D, PeakItem*> _peak_graphics_items;
    nsx::sptrPeak3D _selected_peak;
};

#endif // GUI_GRAPHICS_DETECTORSCENE_H
