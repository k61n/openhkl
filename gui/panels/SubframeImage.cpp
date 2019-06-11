//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeImage.cpp
//! @brief     Implements classes ImageWidget, SubframeImage
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/SubframeImage.h"
#include "core/experiment/DataSet.h"
#include "gui/actions/Triggers.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/MainWin.h"
#include "gui/models/Session.h"
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
    frameLayout = new QFrame(this);
    QHBoxLayout* leftLowerLayout = new QHBoxLayout(frameLayout);
    imageView = new DetectorView;
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
    intensityLayout = new QFrame(this);
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
    mode = new QcrComboBox(
        "adhoc_modus", new QcrCell<int>(1),
        {"selection", "zoom", "line plot", "horizontal slice", "vertical slice", "rectangular mask",
         "ellipsoidal mask"});
    rightLayout->addWidget(mode);
    overallLayout->addLayout(rightLayout);
    mode->setEnabled(false);
    intensityLayout->setEnabled(false);
    frameLayout->setEnabled(false);

    connect(slide, SIGNAL(valueChanged(int)), imageView->getScene(), SLOT(setMaxIntensity(int)));
    connect(slide, &QSlider::valueChanged, [=](int i) { max->setCellValue(i); });
    max->setHook([=](int i) { slide->setValue(i); });
    connect(
        scrollbar, SIGNAL(valueChanged(int)), imageView->getScene(),
        SLOT(slotChangeSelectedFrame(int)));
    connect(scrollbar, &QScrollBar::valueChanged, [=](int i) { frame->setCellValue(i); });
    frame->setHook([=](int i) { scrollbar->setValue(i); });
    mode->setHook([=](int i) { imageView->getScene()->changeInteractionMode(i); });
}

void ImageWidget::dataChanged()
{
    mode->setEnabled(false);
    intensityLayout->setEnabled(false);
    frameLayout->setEnabled(false);
    imageView->getScene()->resetScene();
    if (gSession->selectedExperimentNum() >= 0) {
        nsx::sptrDataSet dataset = gSession->selectedExperiment()->data()->selectedData();
        if (dataset) {
            mode->setEnabled(true);
            intensityLayout->setEnabled(true);
            frameLayout->setEnabled(true);
            int frames = dataset->nFrames();
            scrollbar->setMaximum(frames);
            scrollbar->setMinimum(0);
            scrollbar->setSingleStep(1);
            frame->setMaximum(frames);
            frame->setMinimum(0);
            frame->setSingleStep(1);
            imageView->getScene()->slotChangeSelectedData(dataset, 0);
            slide->setValue(max->getValue());
        }
    }
}

void ImageWidget::changeView(int option)
{
    QTransform trans;
    trans.scale(-1, -1); // fromDetector (default; 0)
    if (option == 1) // fromSample
        trans.scale(1, -1);
    imageView->setTransform(trans);
    imageView->fitScene();
}

//  ***********************************************************************************************

SubframeImage::SubframeImage() : QcrDockWidget {"Image"}
{
    setWidget((centralWidget = new ImageWidget));
    connect(
        this, SIGNAL(visibilityChanged(bool)), &gGui->triggers->viewImage, SLOT(setChecked(bool)));
}
