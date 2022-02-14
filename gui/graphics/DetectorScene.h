//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/DetectorScene.h
//! @brief     Defines class DetectorScene
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_GRAPHICS_DETECTORSCENE_H
#define NSX_GUI_GRAPHICS_DETECTORSCENE_H

#include "base/mask/IMask.h"
#include "core/data/DataTypes.h"
#include "core/detector/DetectorEvent.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "gui/graphics_items/PeakCenterGraphic.h"
#include "gui/models/ColorMap.h"
#include "gui/widgets/PeakViewWidget.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QStack>
#include <qgraphicsitem.h>

namespace nsx {
class UnitCell;
class PeakCenterDataSet;
}
class PeakItemGraphic;
class PeakCollectionModel;
class SXGraphicsItem;

class DirectBeamGraphic : public QGraphicsEllipseItem { }; // Make it easier to remove direct beam

using EventType = nsx::IntegrationRegion::EventType;

// For the plotting part, better to have RowMajor matrix to use QImage scanline
// function and optimize cache hit.
typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> rowMatrix;

//! Master scene of the detector image

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
        VERTICALSLICE = 6
    };

    //! Which mode is the cursor diplaying
    enum CURSORMODE {
        CURSOR = 0, // apparanetly can't have two members named "SELECT" in enums here
        PIXEL = 1,
        THETA = 2,
        GAMMA_NU = 3,
        D_SPACING = 4,
        MILLER_INDICES = 5
    };

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
    //! Set colours for integration region
    void initIntRegionFromPeakWidget(const PeakViewWidget::Set& set, bool alt = false);
    //! Refresh the overlay displaying integration regions
    void refreshIntegrationOverlay();
    //! Generate a mask of integration regions (a matrix of integers classifying pixels)
    void getIntegrationMask(
        PeakCollectionModel* model, Eigen::MatrixXi& mask,
        nsx::RegionType region_type = nsx::RegionType::VariableEllipsoid);
    //! Convert the mask to a QImage with the given colours
    QImage* getIntegrationRegionImage(const Eigen::MatrixXi& mask, QColor& peak, QColor& bkg);
    //! Remove integration overlays from the DetectorScene
    void clearIntegrationRegion();

 public:
    //! Set the first peak model pointer
    void linkPeakModel1(PeakCollectionModel* source);
    //! Set the second peak model pointer
    void linkPeakModel2(PeakCollectionModel* source);
    //! Get the first peak model pointer
    PeakCollectionModel* peakModel1() const;
    //! Get the second peak model pointer
    PeakCollectionModel* peakModel2() const;
    //! Populate vector of 3rd party peak centers
    void link3rdPartyPeaks(nsx::PeakCenterDataSet* pcd);
    //! Set direct beam positions
    void linkDirectBeamPositions(std::vector<nsx::DetectorEvent>* events);
    //! Set unrefined direct beam positions
    void linkOldDirectBeamPositions(std::vector<nsx::DetectorEvent>* events);
    //! Set the first peak model pointer to null
    void unlinkPeakModel1();
    //! Set the second peak model pointer to null
    void unlinkPeakModel2();
    //! Refresh the model data
    void peakModelDataChanged();
    //! Draw the peaks
    void drawPeakitems();
    //! Draw the direct beam position
    void drawDirectBeamPositions();
    //! Draw peaks for one model
    void drawPeakModelItems(PeakCollectionModel* model);
    //! Draw the direct beam position
    void drawDirectBeamPositions(std::vector<nsx::DetectorEvent> events);
    //! Draw peak centers from 3rd party software
    void draw3rdPartyItems();
    //! Remove all the peak elements
    void clearPeakItems();
    //! Set unit cell for Miller Index computation
    void setUnitCell(nsx::UnitCell* cell);
    //! Plot settings for 3rd party peak centres
    void setup3rdPartyPeaks(bool draw, const QColor& color, int size);
    //! Toggle drawing the direct beam position
    void showDirectBeam(bool show);
    //! Get the current intensity
    int intensity() { return _currentIntensity; };

 protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void wheelEvent(QGraphicsSceneWheelEvent* event);

 public slots:
    void resetScene();
    void setMaxIntensity(int);
    void slotChangeSelectedData(nsx::sptrDataSet data, int frame);
    void slotChangeSelectedFrame(int frame);
    void slotChangeEnabledPeak(nsx::Peak3D*) { loadCurrentImage(); }
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
    void signalChangeSelectedPeak(nsx::Peak3D* peak);
    void signalSelectedPeakItemChanged(PeakItemGraphic* peak);
    void signalUpdateDetectorScene();

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
    //! QRect delineating selected area
    QGraphicsRectItem* _selectionRect;
    //! Stack of zoom
    QStack<QRect> _zoomStack;

    //! The current peak model
    PeakCollectionModel* _peak_model_1;
    //! The second peak model (optional, mainly for DetectorWindow)
    PeakCollectionModel* _peak_model_2;
    //! std vector of the peakItems
    std::vector<PeakItemGraphic*> _peak_graphics_items;
    //! std vector of peak centres from 3rd party software
    std::vector<PeakCenterGraphic*> _peak_center_items;
    //! std vector of direct beam positions for each frame
    std::vector<nsx::DetectorEvent>* _direct_beam_events;
    //! direct beam events pre-refinement
    std::vector<nsx::DetectorEvent>* _old_direct_beam_events;

    bool _itemSelected;
    QGraphicsPixmapItem* _image;
    std::vector<std::pair<QGraphicsItem*, nsx::IMask*>> _masks;
    SXGraphicsItem* _lastClickedGI;
    bool _logarithmic;
    bool _drawIntegrationRegion1;
    bool _drawIntegrationRegion2;
    bool _drawDirectBeam;
    bool _draw3rdParty;
    std::unique_ptr<ColorMap> _colormap;
    QGraphicsPixmapItem* _integrationRegion1;
    QGraphicsPixmapItem* _integrationRegion2;
    QGraphicsRectItem* _selected_peak_gi;

    //! Colour of peak pixels in integration region (first overlay)
    QColor _peakPxColor1;
    //! Colour of peak pixels in integration region (second overlay)
    QColor _peakPxColor2;
    //! Colour of background pixels in integration region (first overlay)
    QColor _bkgPxColor1;
    //! Colour of background pixels in integration region (second overlay)
    QColor _bkgPxColor2;
    //! Toggle preview of integration region rather than using regions defined from peaks
    bool _preview_int_regions_1;
    bool _preview_int_regions_2;
    //! Integration region types
    nsx::RegionType _int_region_type_1;
    nsx::RegionType _int_region_type_2;
    //! Integration Region bounds
    double _peak_end_1;
    double _bkg_begin_1;
    double _bkg_end_1;
    double _peak_end_2;
    double _bkg_begin_2;
    double _bkg_end_2;

    //! Colour of 3rd party peaks
    QColor _3rdparty_color;
    //! Size of 3rd party peaks
    int _3rdparty_size;

    //! Colour of direct beam
    QColor _beam_color;
    //! Colour of unrefined direct beam;
    QColor _old_beam_color;
    //! Size of direct beam
    double _beam_size;


    nsx::Peak3D* _selected_peak;

    nsx::UnitCell* _unit_cell;

    nsx::PeakCenterDataSet* _peak_center_data;
};

#endif // NSX_GUI_GRAPHICS_DETECTORSCENE_H
