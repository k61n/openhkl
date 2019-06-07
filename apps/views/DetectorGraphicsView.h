//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/views/DetectorGraphicsView.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QGraphicsView>

class DetectorScene;

class DetectorGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit DetectorGraphicsView(QWidget* parent = 0);
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
