//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/DetectorView.h
//! @brief     Defines class DetectorView
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_DETECTORVIEW_H
#define OHKL_GUI_GRAPHICS_DETECTORVIEW_H

#include <QGraphicsView>

class DetectorScene;

//! View and control the detector image
class DetectorView : public QGraphicsView {
    Q_OBJECT
 public:
    explicit DetectorView(QWidget* parent = 0);
    void resizeEvent(QResizeEvent* event);
    DetectorScene* getScene();
    void keyPressEvent(QKeyEvent* event);
 signals:
 public slots:
    void fitScene();
    void fixDetectorAspectRatio(bool);

 private:
    void copyViewToClipboard();
    DetectorScene* _scene;
};

#endif // OHKL_GUI_GRAPHICS_DETECTORVIEW_H
