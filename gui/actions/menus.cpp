//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/actions/menus.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/actions/menus.h"
#include "gui/actions/triggers.h"
#include "gui/mainwin.h"
#include <QAction>
#include <QMenu>

//! Initialize the menu bar.
Menus::Menus(QMenuBar* mbar) : mbar_ {mbar}
{
    Actions* actions = gGui->triggers;
    mbar->setNativeMenuBar(true);

    actionsToMenu(
        "&File",
        {&actions->addExperiment, &actions->removeExperiment, separator(), &actions->quit});

    QMenu* detector = new QMenu {"&Detector"};
    detector->addActions({&actions->detectorProperties, &actions->goniometer});
    QMenu* sample = new QMenu {"&Sample"};
    sample->addActions(
        {&actions->sampleProperties, &actions->sampleGoniometer, &actions->isotopesDatabase});
    sample->addSeparator()->setText("Shape");
    sample->addActions({&actions->shapeProperties, &actions->shapeLoadMovie});
    QMenu* instrument = new QMenu {"&Instrument"};
    instrument->addMenu(detector);
    instrument->addMenu(sample);
    instrument->addSeparator()->setText("Monochromatic source");
    instrument->addAction(&actions->monochromaticSourceProperties);

    QMenu* data = new QMenu {"&Data"};
    data->addActions({&actions->loadData, &actions->removeData, &actions->dataProperties,
                      &actions->convertHDF5, &actions->importRaw, &actions->instrumentStates,
                      &actions->findPeaks});
    QMenu* indexing = new QMenu {"&indexing"};
    indexing->addActions(
        {&actions->autoIndexer, &actions->userDefinedIndexer, &actions->assignUnitCell});
    QMenu* peaks = new QMenu {"&Peaks"};
    peaks->addAction(&actions->filterPeaks);
    peaks->addMenu(indexing);
    peaks->addActions({&actions->refine, &actions->buildShapeLibrary, &actions->integratepeaks,
                       &actions->normalize, &actions->correctAbsorption, &actions->show3d,
                       &actions->peaksProperties});
    experiment_ = mbar_->addMenu("&Experiments");
    experiment_->addMenu(instrument);
    experiment_->addMenu(data);
    experiment_->addMenu(peaks);
    experiment_->addSeparator()->setText("reference peak library");
    experiment_->addAction(&actions->predictPeaks);
    experiment_->addSeparator()->setText("unit cells");
    experiment_->addAction(&actions->removeUnusedUnitCells);


    actionsToMenu("&Export", {&actions->exportPlot});

    options_ = mbar_->addMenu("&Options");
    QMenu* cursorMode = new QMenu {"&Cursor mode"};
    cursorMode->addActions({&actions->pixelPosition, &actions->gammaNu, &actions->twoTheta,
                            &actions->dSpacing, &actions->millerIndices});
    QMenu* setView = new QMenu {"&Set View"};
    setView->addActions(
        {&actions->fromSample, &actions->behindDetector, &actions->logarithmicScale});
    QMenu* peakMenu = new QMenu {"&Peak"};
    peakMenu->addActions({&actions->showLabels, &actions->showAreas, &actions->drawPeakArea});
    options_->addMenu(cursorMode);
    options_->addMenu(setView);
    options_->addMenu(peakMenu);

    actionsToMenu(
        "&View",
        {&actions->reset, separator(), &actions->viewExperiment, &actions->viewImage,
         &actions->viewLogger, &actions->viewPlotter, &actions->viewProperties});

    actionsToMenu("&Help", {&actions->about});
}

QAction* Menus::separator() const
{
    QAction* ret = new QAction {mbar_};
    ret->setSeparator(true);
    return ret;
}

QMenu* Menus::actionsToMenu(const char* menuName, QList<QAction*> actions)
{
    QMenu* menu = new QMenu {menuName};
    mbar_->addMenu(menu);
    menu->addActions(actions);
    QString prefix = QString("%1: ").arg(menu->title().remove('&'));
    for (auto action : actions)
        action->setToolTip(prefix + action->toolTip());
    return menu;
}
