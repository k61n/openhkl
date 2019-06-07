//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/panels/subframe_image.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "nsxgui/gui/panels/subframe_image.h"
#include "nsxgui/gui/mainwin.h"
#include "nsxgui/gui/view/toggles.h"
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>

//-------------------------------------------------------------------------------------------------

ImageWidget::ImageWidget() : QcrWidget {"Image"}
{
    QHBoxLayout* overallLayout = new QHBoxLayout(this);
    QVBoxLayout* leftLayout = new QVBoxLayout;
    QVBoxLayout* rightLayout = new QVBoxLayout;
    QFrame* frameLayout = new QFrame(this);
    QHBoxLayout* leftLowerLayout = new QHBoxLayout(frameLayout);
    imageScene = new QGraphicsScene;
    imageScene->setBackgroundBrush(Qt::blue);
    imageView = new QGraphicsView(imageScene);
    imageView->show();
    leftLayout->addWidget(imageView);
    scrollbar = new QScrollBar(frameLayout);
    QSizePolicy sizePolicy6(QSizePolicy::Minimum, QSizePolicy::Fixed);
    sizePolicy6.setHorizontalStretch(1);
    sizePolicy6.setVerticalStretch(0);
    sizePolicy6.setHeightForWidth(scrollbar->sizePolicy().hasHeightForWidth());
    scrollbar->setSizePolicy(sizePolicy6);
    scrollbar->setMouseTracking(true);
    scrollbar->setFocusPolicy(Qt::WheelFocus);
    scrollbar->setOrientation(Qt::Horizontal);
    leftLowerLayout->addWidget(scrollbar);
    frame = new QcrSpinBox("frame", new QcrCell<int>(10), 3);
    leftLowerLayout->addWidget(frame);
    leftLayout->addWidget(frameLayout);
    overallLayout->addLayout(leftLayout);
    QFrame* intensityLayout = new QFrame(this);
    QVBoxLayout* verticalLayout = new QVBoxLayout(intensityLayout);
    max = new QcrSpinBox("maxSpin", new QcrCell<int>(10), 3);
    verticalLayout->addWidget(max);
    slide = new QSlider(intensityLayout);
    slide->setMouseTracking(true);
    slide->setAutoFillBackground(false);
    slide->setMinimum(1);
    slide->setMaximum(10000);
    slide->setSingleStep(1);
    slide->setOrientation(Qt::Vertical);
    slide->setTickPosition(QSlider::NoTicks);
    verticalLayout->addWidget(slide);
    rightLayout->addWidget(intensityLayout);
    mode = new QcrComboBox("modus", new QcrCell<int>(0), {"nothing"});
    rightLayout->addWidget(mode);
    overallLayout->addLayout(rightLayout);
}

//  ***********************************************************************************************

SubframeImage::SubframeImage() : QcrDockWidget {"Image"}
{
    setWidget((centralWidget = new ImageWidget));
    connect(
        this, SIGNAL(visibilityChanged(bool)), &gGui->toggles->viewImage, SLOT(setChecked(bool)));
}
