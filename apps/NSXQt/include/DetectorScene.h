#ifndef DETECTORSCENE_H
#define DETECTORSCENE_H

#include <vector>

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
}
class QImage;
class PeakGraphicsItem;
class PeakPlotter;

//! Master Scene containing the pixmap of the detector counts
//! peaks, and other Graphics Items
class DetectorScene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum MODE {ZOOM=0, SELECTION=1, LINE=2, HORIZONTALSLICE=3, VERTICALSLICE=4};
    //! Which mode is the cursor diplaying
    enum CURSORMODE {THETA=0, GAMMA=1, DSPACING=2, PIXEL=3, HKL=4};
    explicit DetectorScene(QObject *parent = 0);
    SX::Data::IData* getData();
signals:
     //! Signal emitted for all changes of the image
    void dataChanged();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
public slots:
    // To be called to update detector image
    void setData(SX::Data::IData*);
    void changeFrame(int frame=0);
    void setMaxIntensity(int);
    //! Change interaction mode in the scene
    void changeInteractionMode(int);
    //!
    void changeCursorMode(int);
private:

    //! Load image from current Data and frame
    void loadCurrentImage();
    //! Create the text of the tooltip depending on Scene Mode.
    void createToolTipText(QGraphicsSceneMouseEvent*);
    //! Point coordinates of the start of zoom region
    QPoint _zoomstart;
    //! Point coordinates of the end of zoom regiom
    QPoint _zoomend;
    //! Graphics Window representing the zoomed area
    QGraphicsRectItem* _zoomrect;
    //! Image of the detector counts
    QGraphicsPixmapItem* _image;
    // Stack of zoom
    QStack<QRect> _zoomStack;
    SX::Data::IData* _currentData;
    int _currentFrameIndex;
    int _currentIntensity;
    std::vector<int> _currentFrame;
    //! Current interaction mode
    MODE _mode;
    CURSORMODE _cursorMode;
    //! Contains peaks item of current data, reinitialized with new data set.
    std::vector<PeakGraphicsItem*> _peaks;
    //!  Peak plotter
    PeakPlotter* _plotter;
};

#endif // DETECTORSCENE_H
