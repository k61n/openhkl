#ifndef DETECTORSCENE_H
#define DETECTORSCENE_H

#include <map>
#include <list>

#include <Eigen/Dense>

#include <QRect>
#include <QStack>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

// Forward declarations
namespace SX
{
namespace Data
{
class IData;
}
namespace Crystal
{
class Peak3D;
}
}
class QImage;
class QGraphicsSceneWheelEvent;
class PeakGraphicsItem;
class CutterGraphicsItem;
class MaskGraphicsItem;
class PlottableGraphicsItem;
class SXGraphicsItem;

//! Master Scene containing the pixmap of the detector counts
//! and overlayed graphics items (peaks, data cutters, masks ...)
class DetectorScene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum MODE {ZOOM=0, LINE=1, HORIZONTALSLICE=2, VERTICALSLICE=3, MASK=4};
    //! Which mode is the cursor diplaying
    enum CURSORMODE {THETA=0, GAMMA=1, DSPACING=2, PIXEL=3, HKL=4};
    explicit DetectorScene(QObject *parent = 0);
    SX::Data::IData* getData();
    const Eigen::MatrixXi& getCurrentFrame() const;
    const std::map<SX::Crystal::Peak3D*,PeakGraphicsItem*>& getPeaksGraphicsItems() const;

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
    void setData(SX::Data::IData*,int frame);
    void setData(SX::Data::IData*);
    void changeFrame(int frame=0);
    void setMaxIntensity(int);
    PeakGraphicsItem* findPeakGraphicsItem(SX::Crystal::Peak3D* peak);
    void updatePeaks();
    //! Change interaction mode in the scene
    void changeInteractionMode(int);
    //!
    void changeCursorMode(int);
    //!
    void showPeakLabels(bool);
    void clearPeaks();
private:

    //! Load image from current Data and frame
    void loadCurrentImage();
    //! Create the text of the tooltip depending on Scene Mode.
    void createToolTipText(QGraphicsSceneMouseEvent*);

    SX::Data::IData* _currentData;
    int _currentFrameIndex;
    int _currentIntensity;
    Eigen::MatrixXi _currentFrame;
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
    std::map<SX::Crystal::Peak3D*,PeakGraphicsItem*> _peaks;
    QList<MaskGraphicsItem*> _masks;
    SXGraphicsItem* _lastClickedGI;
};

#endif // DETECTORSCENE_H
