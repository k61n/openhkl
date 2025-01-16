//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/DetectorScene.h
//! @brief     Defines class DetectorScene
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_DETECTORSCENE_H
#define OHKL_GUI_GRAPHICS_DETECTORSCENE_H

#include "core/data/DataTypes.h"
#include "core/detector/DetectorEvent.h"
#include "gui/graphics/DataSetGraphics.h"
#include "gui/graphics/DetectorSceneParams.h"
#include "gui/graphics/PeakCollectionGraphics.h"

#include <QGraphicsScene>
#include <QStack>

#include <opencv2/core/types.hpp>

namespace ohkl {
class KeyPointCollection;
class Peak3D;
class PeakCenterDataSet;
class UnitCell;
}
class CircleGraphic;
class CrosshairGraphic;
class CutterItem;
class PeakItemGraphic;
class PeakCollectionModel;
class PeakViewWidget;
class MaskItem;
class SXGraphicsItem;

class QGraphicsPixmapItem;

//! Master Scene containing the pixmap of the detector counts
//! and overlayed graphics items (peaks, data cutters, masks ...)
class DetectorScene : public QGraphicsScene {
    Q_OBJECT
 public:
    enum MODE {
        ZOOM = 0,
        SELECT = 1,
        MASK = 2,
        ELLIPSE_MASK = 3,
        LINE = 4,
        HORIZONTALSLICE = 5,
        VERTICALSLICE = 6,
        DRAG_DROP = 7,
        INTENSITY_HISTO = 8
    };

    explicit DetectorScene(std::size_t npeakcollections, QObject* parent = 0);

    ohkl::sptrDataSet getData() { return _currentData; };
    //! Get the current frame
    int currentFrame() const { return _currentFrameIndex; }
    //! Get the matrix of current frame data
    RowMatrix currentFrameMatrix() const { return _dataset_graphics->currentFrame(); };
    //! Get a pointer to the DetectorSceneParams
    DetectorSceneParams* params() { return &_params; };
    //! Load image from current Data and frame
    void loadCurrentImage();
    //! Remove integration overlays from the DetectorScene
    void clearPixmapItems();
    //! Remove masks
    void clearMasks();
    //! Remove circles
    void clearCircles();

    //! Get pointer to a PeakCollectionGraphics object
    PeakCollectionGraphics* peakCollectionGraphics(std::size_t idx) const;
    //! Link graphics associated with a peak collection
    void linkPeakModel(PeakCollectionModel* source, PeakViewWidget* widget, std::size_t idx = 0);
    //! Set visualisation type for the subframe
    void setVisualisationMode(VisualisationType vtype);
    //! Populate vector of 3rd party peak centers
    void link3rdPartyPeaks(ohkl::PeakCenterDataSet* pcd, std::size_t idx);
    //! Link data for keypoints found via OpenCV
    void linkKeyPoints(ohkl::KeyPointCollection* points, std::size_t idx);
    //! Set refined and unrefined beam positions
    void linkDirectBeam(
        std::vector<ohkl::DetectorEvent>* beam, std::vector<ohkl::DetectorEvent>* old);
    //! Refresh the model data
    void peakModelDataChanged();
    //! Draw the peaks
    void drawPeakItems();
    //! Remove all the peak elements
    void clearPeakItems();
    //! Set unit cell for Miller Index computation
    void setUnitCell(ohkl::UnitCell* cell) { _dataset_graphics->setUnitCell(cell); };
    //! Get the current intensity
    int intensity() { return _params.intensity; };
    //! Set up the direct beam crosshair
    void addBeamSetter(const QPointF& pos, int size);
    //! Add a circle of given position and radius
    CircleGraphic* addCircle(const QPointF& pos, int radius);
    //! Remove the beam crosshair from the scene
    void removeBeamSetter();
    //! Show/hide the beam setter crosshair
    void showBeamSetter(bool show);
    //! Get the beam setter crosshairs
    CrosshairGraphic* beamSetter() const { return _beam_pos_setter; };
    //! Get the beam setter coordinates
    static QPointF beamSetterCoords() { return _current_beam_position; };
    //! Return the interaction mode
    int mode() const { return static_cast<int>(_mode); };
    //! Set single peak for single peak integration overlay
    void setPeak(ohkl::Peak3D* peak);
    //! Load masks from current DataSet
    void loadMasksFromData();
    //! Get pointers to all mask items in the scene
    QVector<MaskItem*> maskItems() const;
    //! Remove text from scene (contour & peak labels)
    void clearText();
    //! Send a signal when the direct beam position crosshair is changed
    void sendBeamOffset(QPointF pos);

 protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void wheelEvent(QGraphicsSceneWheelEvent* event);

 public slots:
    void onGradientSetting(int kernel);
    void setMaxIntensity(int);
    void slotChangeSelectedData(ohkl::sptrDataSet data, int frame_1based);
    void slotChangeSelectedFrame(int frame_1based);
    void slotChangeEnabledPeak(ohkl::Peak3D*) { loadCurrentImage(); }
    void slotChangeMaskedPeaks(const ohkl::PeakList&) { loadCurrentImage(); }
    void changeInteractionMode(int mode) { _mode = static_cast<MODE>(mode); }
    void changeCursorMode(int mode) { _params.tooltipMode = static_cast<TooltipMode>(mode); };
    void drawIntegrationRegion();
    void updateMasks() { _lastClickedGI = nullptr; }
    void setBeamSetterPos(QPointF pos);
    void onCrosshairResized(int size);

 signals:
    void dataChanged();
    void signalChangeSelectedData(ohkl::sptrDataSet data);
    void signalChangeSelectedFrame(int selected_frame);
    void signalChangeSelectedPeak(ohkl::Peak3D* peak);
    void signalSelectedPeakItemChanged(PeakItemGraphic* peak);
    void signalUpdateDetectorScene();
    void signalPeakSelected(ohkl::Peak3D* peak);
    void signalMaskChanged();
    void beamPosChanged(QPointF pos);
    void signalMasksSelected();

 private:
    //! Create the text of the tooltip depending on Scene Mode.
    void createToolTipText(QGraphicsSceneMouseEvent*);
    //! Ensure the bounds for a drag/dropped box are physical
    bool setBoxBounds(QGraphicsRectItem* box);
    //! Adjust zoom rect bounds if they are at edge of image
    void adjustZoomRect(QGraphicsRectItem* box);
    //! Check whether a graphics item exists, if it does, remove + delete
    void deleteGraphicsItem(QGraphicsItem* item);
    //! Draw the resolution contours
    void drawResolutionContours();
    //! Check whether current event was a drag
    bool isDrag(const QPointF& current);

    //! Visual parameters of the scene
    DetectorSceneParams _params;
    //! Pointer to the current DataSet
    ohkl::sptrDataSet _currentData;
    //! Integer index of the frame being displayed
    unsigned int _currentFrameIndex;
    //! Current interaction mode
    MODE _mode;
    //! Point coordinates of the start of zoom region
    QPoint _zoomstart;
    //! Point coordinates of the end of zoom regiom
    QPoint _zoomend;
    //! Graphics Window representing the zoomed area
    QGraphicsRectItem* _zoomrect;
    //! QRect delineating selected area
    QGraphicsRectItem* _selectionRect;
    //! Stack of zoom
    QStack<QRect> _zoomStack;
    //! item being dragged
    CrosshairGraphic* _current_dragged_item;
    //! Click position to determine whether dragging occured
    QPointF _clickPos;
    //! Determines whether a mousePressEvent happened
    bool _clicked;
    //! determines whether a mouseMoveEvent happened
    bool _dragged;

    QGraphicsPixmapItem* _image;
    SXGraphicsItem* _lastClickedGI;
    QGraphicsRectItem* _selected_peak_gi;

    //! Object storing all data set-related graphics
    std::unique_ptr<DataSetGraphics> _dataset_graphics;
    //! Object storing all peak-related graphics
    std::vector<std::unique_ptr<PeakCollectionGraphics>> _peak_graphics;
    //! Maximum number of peak collections for this scene
    unsigned int _max_peak_collections;
    //! Store for pointers to integration overlays
    QVector<QGraphicsPixmapItem*> _integration_regions;

    //! Crosshair for setting direct beam
    CrosshairGraphic* _beam_pos_setter;
    //! current position of the crosshair
    static QPointF _current_beam_position;

    //! Object for computing intensity profiles on a line through the scene
    CutterItem* _cutter;

    //! Peak selected by interacting with the DetectorScene
    PeakItemGraphic* _selected_peak;
    //! Selected peaks for drawing a single integration region
    ohkl::Peak3D* _peak;

    //! QImage overlay of resolution contours
    std::optional<QImage> _contours;
    //! The contour overlay item
    QGraphicsPixmapItem* _contour_overlay;
};

#endif // OHKL_GUI_GRAPHICS_DETECTORSCENE_H
