//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/actions/Triggers.cpp
//! @brief     Implements class Actions
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/actions/Triggers.h"

#include "gui/dialogs/ListNameDialog.h"
#include "gui/dialogs/PeakFilterDialog.h"
#include "gui/dialogs/ShapeLibraryDialog.h"
#include "gui/frames/AutoIndexerFrame.h"
#include "gui/frames/GlobalOffsetsFrame.h"
#include "gui/frames/InstrumentStatesFrame.h"
#include "gui/frames/PeakFinderFrame.h"
#include "gui/frames/RefinerFrame.h"
#include "gui/frames/UserDefinedUnitCellIndexerFrame.h"
#include "gui/MainWin.h"
#include "gui/models/Session.h" //for gSession
#include "gui/panels/SubframeSetup.h"
#include "gui/panels/TabInstrument.h"

#include <QDesktopServices>
#include <QInputDialog>

Actions::Actions()
{
    setupData();
    setupExperiment();
    setupPeaks();
    setupFiles();
    setupInstrument();
    setupOptions();
    setupRest();
}

void Actions::setupFiles()
{
    addExperiment.setTriggerHook([]() { gSession->createExperiment(); });
    quit.setTriggerHook([]() { gGui->close(); });
    removeExperiment.setTriggerHook([]() { gSession->removeExperiment(); });
}

void Actions::setupData()
{
    dataProperties.setTriggerHook([]() {
        SubframeSetup* properties = gGui->dockProperties_->tabsframe;
        int i = properties->indexOf(properties->data);
        properties->setCurrent(i);
    });
    loadData.setTriggerHook([]() { gSession->loadData(); });
    removeData.setTriggerHook([]() { gSession->removeData(); });
    importRaw.setTriggerHook([]() { gSession->loadRawData(); });
    findPeaks.setTriggerHook([]() { new PeakFinder; });
    instrumentStates.setTriggerHook([]() { new InstrumentStates; });
}

void Actions::setupExperiment() {}

void Actions::setupInstrument()
{
    monochromaticSourceProperties.setTriggerHook([]() {
        SubframeSetup* properties = gGui->dockProperties_->tabsframe;
        TabInstrument* tab = properties->instrument;
        int i = tab->indexOf(tab->monoSource);
        properties->setCurrent(0);
        tab->setCurrent(i);
    });
    shapeProperties.setTriggerHook([]() {
        SubframeSetup* properties = gGui->dockProperties_->tabsframe;
        TabInstrument* tab = properties->instrument;
        int i = tab->indexOf(tab->sample);
        properties->setCurrent(0);
        tab->setCurrent(i);
    });
    sampleProperties.setTriggerHook([]() {
        SubframeSetup* properties = gGui->dockProperties_->tabsframe;
        TabInstrument* tab = properties->instrument;
        int i = tab->indexOf(tab->sample);
        properties->setCurrent(0);
        tab->setCurrent(i);
    });
    detectorProperties.setTriggerHook([]() {
        SubframeSetup* properties = gGui->dockProperties_->tabsframe;
        TabInstrument* tab = properties->instrument;
        int i = tab->indexOf(tab->detector);
        properties->setCurrent(0);
        tab->setCurrent(i);
    });
    goniometer.setTriggerHook([]() { new GlobalOffsets(offsetMode::DETECTOR); });
    sampleGoniometer.setTriggerHook([]() { new GlobalOffsets(offsetMode::SAMPLE); });
}

void Actions::setupOptions()
{
    fromSample.setTriggerHook([](){ gGui->changeView(1); });
    behindDetector.setTriggerHook([](){ gGui->changeView(0); });
    pixelPosition.setTriggerHook([](){ gGui->cursormode(0); });
    twoTheta.setTriggerHook([](){ gGui->cursormode(1); });
    gammaNu.setTriggerHook([](){ gGui->cursormode(2); });
    dSpacing.setTriggerHook([](){ gGui->cursormode(3); });
    millerIndices.setTriggerHook([](){ gGui->cursormode(4); });
    logarithmicScale.setHook([](bool checked){
        gGui->dockImage_->centralWidget->imageView->getScene()->setLogarithmic(checked);
    });
}

void Actions::setupPeaks()
{
    peaksProperties.setTriggerHook([]() {
        SubframeSetup* properties = gGui->dockProperties_->tabsframe;
        int i = properties->indexOf(properties->peaks);
        properties->setCurrent(i);
    });
    autoIndexer.setTriggerHook([]() { new AutoIndexer; });
    filterPeaks.setTriggerHook([]() { new PeakFilter; });
    userDefinedIndexer.setTriggerHook([]() { new UserDefinedUnitCellIndexer; });
    assignUnitCell.setTriggerHook([]() {
        if (gSession->selectedExperimentNum() < 0) {
            gLogger->log("[ERROR] No experiment selected");
            return;
        }
        if (gSession->selectedExperiment()->peaks()->allPeaks().empty()) {
            gLogger->log("[ERROR] No peaks in selected experiment");
            return;
        }
        // gSession->selectedExperiment()->peaks()->autoAssignUnitCell();
    });
    buildShapeLibrary.setTriggerHook([]() { new ShapeLibraryDialog; });
    refine.setTriggerHook([]() { new Refiner; });
    normalize.setTriggerHook([](){
        if (gSession->selectedExperimentNum() < 0)
            return;
        gSession->selectedExperiment()->peaks()->normalizeToMonitor();
    });
}

void Actions::setupRest()
{
    reset.setTriggerHook([]() { gGui->resetViews(); });
    viewExperiment.setHook([](bool check) { gGui->dockExperiments_->setVisible(check); });
    viewImage.setHook([](bool check) { gGui->dockImage_->setVisible(check); });
    viewLogger.setHook([](bool check) { gGui->dockLogger_->setVisible(check); });
    viewPlotter.setHook([](bool check) { gGui->dockPlot_->setVisible(check); });
    viewProperties.setHook([](bool check) { gGui->dockProperties_->setVisible(check); });
    exportPlot.setTriggerHook([](){ gGui->exportPlot(); });
}
