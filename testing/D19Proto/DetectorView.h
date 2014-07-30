#ifndef DETECTORVIEW_H
#define DETECTORVIEW_H
#include <QGraphicsView>
#include <QWidget>
#include <QMouseEvent>
#include <QToolTip>
#include "Data.h"
#include "Plotter1D.h"
#include <QStack>
#include "PeakPlotter.h"

class DetectorView : public QGraphicsView
{
    Q_OBJECT
public:
    enum CrossMode {THETA, GAMMA, D, PIXEL};
    //
    enum CutterMode {ZOOM=0, LINE=1, ELLIPSE=2, HORIZONTALSLICE=3, VERTICALSLICE=4};

    DetectorView(QWidget* parent);
    // Set the number of pixels in the detector
    void setNpixels(int hor,int vert);
    // Set the dimensions of the detector
    void setDimensions(double gammaRange, double height);
    // Set the detector distance
    void setDetectorDistance(double distance);
    // Is Data present
    bool hasData() const;
public slots:
    void updateView(Data* ptr,int frame);
    void setCutterMode(int i);
    // Set the zoom window to (x1,x2,y1,y2) in detector space coordinates;
    void setZoom(int x1,int y1, int x2, int y2);
    void setMaxIntensity(int);
    // Register this zoom level in the stack
    void registerZoomLevel(int xmin, int xmax, int ymin, int ymax);
    // Go to previous zoom level
    void setPreviousZoomLevel();
    // Integrate in vertical direction
    void integrateVertical(int xmin,int xmax, int ymin, int ymax, QVector<double>& projection, QVector<double>& error);
    // Integrate in horizontal direction
    void integrateHorizontal(int xmin,int xmax, int ymin, int ymax, QVector<double>& projection, QVector<double>& error);
protected:
    // Mouse events
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);
    void detectorToScene(double& x, double& y);
    void sceneToDetector(double& x, double& y);
    void keyPressEvent(QKeyEvent* event);
    void wheelEvent(QWheelEvent *event);
private:
    //
    void plotIntensityMap();
    //
    void plotEllipsoids();
    //
    void updatePlot();
    // Get gamma,nu in degrees for a pixel at (x,y) in detector space
    void getGammaNu(double x, double y, double& gamma, double& nu);
    // Get 2theta in degrees for a pixel (x,y) in detector space.
    void get2Theta(double x, double y, double& th2);
    // Get d-spacing for a pixel (x,y) in detector space
    void getDSpacing(double x, double y, double& dspacing);
    // Update line cutter, x and y in scene coordinates
    void updateLineCutter();
    // Update zoom cutter, x and y in scene coordinates
    void updateZoomCutter(const QPointF&);
    // Check pixel x y is in the detector scene
    bool pointInScene(const QPointF&);
    //
    void updateSliceIntegrator();
    // Pointer to Data
    Data* _ptrData;
    int pixels_h;
    int pixels_v;
    // Width of detector in gamma
    double _gammawidth;
    // Height of detector in m
    double _height;
    // Distance of detector
    double _distance;
    // Pointer to the scene
    QGraphicsScene* _scene;
    // Mode for cursor information
    CrossMode _mode;
    //
    CutterMode _cutterMode;
    QGraphicsLineItem* _line;
    QGraphicsRectItem *_zoom, *_slice;
    //
    QImage* _currentImage;

    //
    Plotter1D* _cutPloter;
    // Zoom corners in detector space coordinates.
    int _zoomLeft, _zoomTop, _zoomRight, _zoomBottom;
    double _maxIntensity;
    // Maintain the stack of zoom levels
    QStack<QRect> _zoomStack;
    int _currentFrame;
    double _previousX, _previousY;
    Plotter1D* _plotter;
    int _sliceThickness;
    QGraphicsPixmapItem *_pixmap;
    QList<QGraphicsRectItem*> _currentPeaks;
    PeakPlotter* _peakplotter;
    double _dr;
};

#endif // DETECTORVIEW_H
