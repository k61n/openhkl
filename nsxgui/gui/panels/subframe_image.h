
#ifndef SUBFRAME_IMAGE_H
#define SUBFRAME_IMAGE_H

#include <QCR/widgets/views.h>
#include <QCR/widgets/controls.h>
#include <QGraphicsView>

//! Subframe of the main window, with tabs to control detector, fits, &c

class ImageWidget : public QcrWidget {
public:
    ImageWidget();
private:
    QcrSpinBox* max;
    QcrSpinBox* frame;
    QcrComboBox* mode;
    QSlider* slide;
    QScrollBar* scrollbar;
    QGraphicsView* imageView;
    QGraphicsScene* imageScene;
};

class SubframeImage : public QcrDockWidget {
public:
    SubframeImage();
private:
    ImageWidget* centralWidget;
};

#endif // SUBFRAME_IMAGE_H
