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

#include "base/mask/IMask.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/detector/DetectorEvent.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "gui/graphics/DataSetGraphics.h"
#include "gui/graphics/DetectorSceneParams.h"
#include "gui/graphics/PeakCollectionGraphics.h"
#include "gui/graphics_items/CrosshairGraphic.h"
#include "gui/graphics_items/PeakCenterGraphic.h"
#include "gui/models/ColorMap.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QStack>

#include <opencv2/core/types.hpp>

namespace ohkl {
class UnitCell;
class PeakCenterDataSet;
class MaskHandler;
}
class CutterItem;
class PeakItemGraphic;
class PeakCollectionModel;
class PeakViewWidget;
class SXGraphicsItem;
class MaskHandler;

class DirectBeamGraphic : public QGraphicsEllipseItem { }; // Make it easier to remove direct beam

using EventType = ohkl::IntegrationRegion::EventType;

// For the plotting part, better to have RowMajor matrix to use QImage scanline
// function and optimize cache hit.
typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> rowMatrix;

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
    const rowMatrix& getCurrentFrame() const { return _currentFrame; };
    void setLogarithmic(bool checked) { _params.logarithmic = checked; };
    void setGradient(bool checked) { _params.gradient = checked; };
    //! Get a pointer to the DetectorSceneParams
    DetectorSceneParams* params() { return &_params; };
    //! Load image from current Data and frame
    void loadCurrentImage();
    //! Remove integration overlays from the DetectorScene
    void clearIntegrationRegion();
    //! Remove masks
    void clearMasks();

    //! Get pointer to a PeakCollectionGraphics object
    PeakCollectionGraphics* peakCollectionGraphics(std::size_t idx) const;
    //! Link graphics associated with a peak collection
    void linkPeakModel(PeakCollectionModel* source, PeakViewWidget* widget, std::size_t idx = 0);
    //! Set visualisation type for the subframe
    void setVisualisationMode(VisualisationType vtype);
    //! Populate vector of 3rd party peak centers
    void link3rdPartyPeaks(ohkl::PeakCenterDataSet* pcd, std::size_t idx);
    //! Link data for keypoints found via OpenCV
    void linkPerFrameSpots(std::vector<std::vector<cv::KeyPoint>>* points, std::size_t idx);
    //! Set direct beam positions
    void linkDirectBeamPositions(std::vector<ohkl::DetectorEvent>* events);
    //! Set unrefined direct beam positions
    void linkOldDirectBeamPositions(std::vector<ohkl::DetectorEvent>* events);
    //! Refresh the model data
    void peakModelDataChanged();
    //! Draw the peaks
    void drawPeakItems();
    //! Draw the direct beam position
    void drawDirectBeamPositions();
    //! Remove all the peak elements
    void clearPeakItems();
    //! Set unit cell for Miller Index computation
    void setUnitCell(ohkl::UnitCell* cell);
    //! Toggle drawing the direct beam position
    void showDirectBeam(bool show);
    //! Get the current intensity
    int intensity() { return _params.intensity; };
    //! Set up the direct beam crosshair
    void addBeamSetter(int size, int linewidth);
    //! Remove the beam crosshair from the scene
    void removeBeamSetter();
    //! Show/hide the beam setter crosshair
    void showBeamSetter(bool show);
    //! Get the beam setter position
    Eigen::Vector3d getBeamSetterPosition() const;
    //! Get the beam setter crosshairs
    CrosshairGraphic* beamSetter() const { return _beam_pos_setter; };
    //! Get the beam setter coordinates
    static QPointF beamSetterCoords();
    //! Return the interaction mode
    int mode() const { return static_cast<int>(_mode); };
    //! Set single peak for single peak integration overlay
    void setPeak(ohkl::Peak3D* peak);
    //! Set scene to draw integration for single peak
    //! Load masks from current DataSet
    void loadMasksFromData();
    //! Add masks to the detector image
    void addMasks();

    std::shared_ptr<MaskHandler> getMaskHandler();

 protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void wheelEvent(QGraphicsSceneWheelEvent* event);

 public slots:
    void onGradientSetting(int kernel, bool fft);
    void resetElements();
    void resetScene();
    void setMaxIntensity(int);
    void slotChangeSelectedData(ohkl::sptrDataSet data, int frame_1based);
    void slotChangeSelectedFrame(int frame_1based);
    void slotChangeEnabledPeak(ohkl::Peak3D*) { loadCurrentImage(); }
    void slotChangeMaskedPeaks(const ohkl::PeakList&) { loadCurrentImage(); }
    void changeInteractionMode(int mode) { _mode = static_cast<MODE>(mode); }
    void changeCursorMode(int mode) { _params.tooltipMode = static_cast<TooltipMode>(mode); };
    void drawIntegrationRegion();
    void updateMasks() { _lastClickedGI = nullptr; }
    int currentFrame() const { return _currentFrameIndex; }
    void setBeamSetterPos(QPointF pos);
    void onCrosshairChanged(int size, int linewidth);
    void toggleMasks();

 signals:
    //! Signal emitted for all changes of the image
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

    //! Visual parameters of the scene
    DetectorSceneParams _params;
    //! Pointer to the current DataSet
    ohkl::sptrDataSet _currentData;
    //! Integer index of the frame being displayed
    unsigned int _currentFrameIndex;
    //! Raw matrix for the current image
    rowMatrix _currentFrame;
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

    //! std vector of direct beam positions for each frame
    std::vector<ohkl::DetectorEvent>* _direct_beam_events;
    //! direct beam events pre-refinement
    std::vector<ohkl::DetectorEvent>* _old_direct_beam_events;

    bool _itemSelected;
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

    //! Colour of direct beam
    QColor _beam_color;
    //! Colour of unrefined direct beam;
    QColor _old_beam_color;
    //! Size of direct beam
    double _beam_size;
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

    std::shared_ptr<MaskHandler> _mask_handler;
};

#endif // OHKL_GUI_GRAPHICS_DETECTORSCENE_H
