#ifndef DETECTORGRAPHICSVIEW_H
#define DETECTORGRAPHICSVIEW_H

#include <QGraphicsView>

class DetectorScene;
class DetectorGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit DetectorGraphicsView(QWidget *parent = 0);
    void resizeEvent(QResizeEvent *event);
    DetectorScene* getScene();
signals:

public slots:

private:
    DetectorScene* _scene;
};

#endif // DETECTORGRAPHICSVIEW_H
