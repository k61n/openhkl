#ifndef NSXQT_DETECTORSCENE_H
#define NSXQT_DETECTORSCENE_H

#include <map>
#include <vector>
#include <utility>

#include <Eigen/Dense>

#include <QRect>
#include <QStack>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include <nsxlib/crystal/CrystalTypes.h>
#include <nsxlib/crystal/PeakCalc.h>
#include <nsxlib/data/DataTypes.h>
#include <nsxlib/geometry/IMask.h>

#include "items/PeakCalcGraphicsItem.h"

#include "ColorMap.h"

class QImage;
class QGraphicsSceneWheelEvent;
class PeakGraphicsItem;
class CutterGraphicsItem;
class MaskGraphicsItem;
class EllipseMaskGraphicsItem;
class PlottableGraphicsItem;
class SXGraphicsItem;

// For the plotting part, better to have RowMajor matrix to use QImage scanline function and
// optimize cache hit.
typedef Eigen::Matrix<int,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> rowMatrix;

//! Master Scene containing the pixmap of the detector counts
//! and overlayed graphics items (peaks, data cutters, masks ...)
class DetectorScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum MODE {ZOOM=0, LINE=1, HORIZONTALSLICE=2, VERTICALSLICE=3, MASK=4,ELLIPSE_MASK = 5,INDEXING=6};
    //! Which mode is the cursor diplaying
    enum CURSORMODE {THETA=0, GAMMA=1, DSPACING=2, PIXEL=3, HKL=4};
    explicit DetectorScene(QObject *parent = 0);
    nsx::sptrDataSet getData();
    const rowMatrix& getCurrentFrame() const;
    void setLogarithmic(bool checked);
    void setColorMap(const std::string& name);

signals:
     //! Signal emitted for all changes of the image
    void dataChanged();
    void updatePlot(PlottableGraphicsItem* cutter);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);

public slots:
    void resetScene();
    // To be called to update detector image
    void setData(const nsx::sptrDataSet&, size_t frame);
    void setData(const nsx::sptrDataSet&);
    void changeFrame(size_t frame = 0);
    void setMaxIntensity(int);

    PeakGraphicsItem* findPeakGraphicsItem(const nsx::sptrPeak3D& peak);

    void updatePeaks();
    void updatePeakCalcs();
    void redrawImage();
    //! Change interaction mode in the scene
    void changeInteractionMode(int);
    //!
    void changeCursorMode(int);
    //!
    void showPeakLabels(bool);

    void drawIntegrationRegion(bool);

    void clearPeaks();

    void updateMasks(unsigned long frame);

private:
    //! Load image from current Data and frame
    void loadCurrentImage(bool newimage=true);
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
    // Stack of zoom
    QStack<QRect> _zoomStack;
    bool _itemSelected;
    QGraphicsPixmapItem* _image;
    //! Contains peaks item of current data, reinitialized with new data set.
    std::map<nsx::sptrPeak3D,PeakGraphicsItem*> _peakGraphicsItems;
    std::vector<PeakCalcGraphicsItem*> _peakCalcs;
    std::vector<std::pair<QGraphicsItem*, nsx::IMask*>> _masks;
    
    SXGraphicsItem* _lastClickedGI;
    nsx::PeakCalcList _precalculatedPeaks;

    bool _logarithmic;
    bool _drawIntegrationRegion;
    std::unique_ptr<ColorMap> _colormap;

    QGraphicsPixmapItem* _integrationRegion;
};

#endif // NSXQT_DETECTORSCENE_H
