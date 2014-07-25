#ifndef DETECTORVIEW_H
#define DETECTORVIEW_H
#include <QGraphicsView>
#include <QWidget>
#include <QMouseEvent>
#include <QToolTip>
#include "Data.h"



class DetectorView : public QGraphicsView
{
public:
    enum CrossMode {THETA, GAMMA, D, PIXEL};

    DetectorView(QWidget* parent);
    // Set the number of pixels in the detector
    void setNpixels(int hor,int vert);
    // Set the dimensions of the detector
    void setDimensions(double gammaRange, double height);
    // Set the detector distance
    void setDetectorDistance(double distance);
public slots:
    void updateView(Data* ptr,int frame, double colormax);

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void detectorToScene(double& x, double& y);
    void sceneToDetector(double& x, double& y);
    void keyPressEvent(QKeyEvent* event);
private:
    // Get gamma,nu in degrees for a pixel at (x,y) in detector space
    void getGammaNu(double x, double y, double& gamma, double& nu);
    // Get 2theta in degrees for a pixel (x,y) in detector space.
    void get2Theta(double x, double y, double& th2);
    // Get d-spacing for a pixel (x,y) in detector space
    void getDSpacing(double x, double y, double& dspacing);
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
};

#endif // DETECTORVIEW_H
