#ifndef DETECTORSCENE_H
#define DETECTORSCENE_H

#include <vector>

#include <QRect>
#include <QStack>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

namespace SX
{
    namespace Data
    {
        class IData;
    }
}

using namespace SX::Data;

class QImage;
class DetectorScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit DetectorScene(QObject *parent = 0);

signals:
    void hasReceivedData();
    //! Signal emitted when the user change zoomed area
    void zoomChanged();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
public slots:
    // To be called to update detector image
    void setData(IData*);
    void setCurrentFrame(int frame);
    void changeFrame(int frame=0);
    void setMaxIntensity(int);
private:
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
    IData* _currentData;
    int _currentFrameIndex;
    int _currentIntensity;
    std::vector<int> _currentFrame;
};

#endif // DETECTORSCENE_H
