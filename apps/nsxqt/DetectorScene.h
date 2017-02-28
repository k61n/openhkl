#ifndef DETECTORSCENE_H
#define DETECTORSCENE_H

#include <map>
#include <list>
#include <memory>

#include <Eigen/Dense>

#include <QRect>
#include <QStack>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <memory>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/crystal/Indexer.h>
#include <nsxlib/crystal/PeakCalc.h>
#include "items/PeakCalcGraphicsItem.h"

#include "ColorMap.h"

// Forward declarations

namespace SX
{
namespace Data
{
class DataSetet;
}
namespace Crystal
{
class Peak3D;
using sptrPeak3D = std::shared_ptr<Peak3D>;
}
}
class QImage;
class QGraphicsSceneWheelEvent;
class PeakGraphicsItem;
class CutterGraphicsItem;
class MaskGraphicsItem;
class PlottableGraphicsItem;
class SXGraphicsItem;

// For the plotting part, better to have RowMajor matrix to use QImage scanline function and
// optimize cache hit.
typedef Eigen::Matrix<int,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> rowMatrix;

using SX::Crystal::sptrPeak3D;

//! Master Scene containing the pixmap of the detector counts
//! and overlayed graphics items (peaks, data cutters, masks ...)
class DetectorScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum MODE {ZOOM=0, LINE=1, HORIZONTALSLICE=2, VERTICALSLICE=3, MASK=4,INDEXING=5};
    //! Which mode is the cursor diplaying
    enum CURSORMODE {THETA=0, GAMMA=1, DSPACING=2, PIXEL=3, HKL=4};
    explicit DetectorScene(QObject *parent = 0);
    std::shared_ptr<SX::Data::DataSetet> getData();
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
    // To be called to update detector image
    void setData(const std::shared_ptr<SX::Data::DataSetet>&, size_t frame);
    void setData(const std::shared_ptr<SX::Data::DataSetet>&);
    void changeFrame(size_t frame = 0);
    void setMaxIntensity(int);
    PeakGraphicsItem* findPeakGraphicsItem(const sptrPeak3D& peak);
//    void setPeakIndex(sptrPeak3D peak,const Eigen::Vector3d& index);
    void updatePeaks();
    void updatePeakCalcs();
    void redrawImage();
    //! Change interaction mode in the scene
    void changeInteractionMode(int);
    //!
    void changeCursorMode(int);
    //!
    void showPeakLabels(bool);
    void drawPeakBackground(bool);
    //!
    void showPeakCalcs(bool);
    void clearPeaks();

    void updateMasks(unsigned long frame);

private:
    //! Load image from current Data and frame
    void loadCurrentImage(bool newimage=true);
    //! Create the text of the tooltip depending on Scene Mode.
    void createToolTipText(QGraphicsSceneMouseEvent*);

    std::shared_ptr<SX::Data::DataSetet> _currentData;
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
    std::map<sptrPeak3D,PeakGraphicsItem*> _peakGraphicsItems;
    std::vector<PeakCalcGraphicsItem*> _peakCalcs;
    QList<MaskGraphicsItem*> _masks;
    SXGraphicsItem* _lastClickedGI;
    std::unique_ptr<SX::Crystal::Indexer> _indexer;
    std::vector<SX::Crystal::PeakCalc> _precalculatedPeaks;

    bool _showPeakCalcs;
    bool _logarithmic;
    std::unique_ptr<ColorMap> _colormap;
};

#endif // DETECTORSCENE_H
