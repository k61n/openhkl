
#ifndef GUI_GRAPHICS_DETECTORVIEW_H
#define GUI_GRAPHICS_DETECTORVIEW_H

#include <QGraphicsView>

class XDetectorScene;

class DetectorView : public QGraphicsView {
    Q_OBJECT
public:
    explicit DetectorView(QWidget* parent = 0);
    void resizeEvent(QResizeEvent* event);
    XDetectorScene* getScene();
    void keyPressEvent(QKeyEvent* event);
signals:
public slots:
    void fitScene();
    void fixDetectorAspectRatio(bool);

private:
    void copyViewToClipboard();
    XDetectorScene* _scene;
};

#endif //GUI_GRAPHICS_DETECTORVIEW_H
