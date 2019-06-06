//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/actions/triggers.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/actions/triggers.h"
#include "gui/dialogs/listnamedialog.h"
#include "gui/dialogs/peakfilter.h"
#include "gui/dialogs/shapelibrary.h"
#include "gui/frames/autoindexer.h"
#include "gui/frames/globaloffsets.h"
#include "gui/frames/instrumentstates.h"
#include "gui/frames/peakfinder.h"
#include "gui/frames/refiner.h"
#include "gui/frames/userdefinedunitcellindexer.h"
#include "gui/mainwin.h"
#include "gui/models/session.h" //for gSession
#include "gui/panels/subframe_setup.h"
#include "gui/panels/tab_instrument.h"

#include <QDesktopServices>
#include <QInputDialog>

Triggers::Triggers()
{
    reset.setTriggerHook([]() { gGui->resetViews(); });
    quit.setTriggerHook([]() { gGui->deleteLater(); });
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
    peaksProperties.setTriggerHook([]() {
        SubframeSetup* properties = gGui->dockProperties_->tabsframe;
        int i = properties->indexOf(properties->peaks);
        properties->setCurrent(i);
    });
    dataProperties.setTriggerHook([]() {
        SubframeSetup* properties = gGui->dockProperties_->tabsframe;
        int i = properties->indexOf(properties->data);
        properties->setCurrent(i);
    });
    loadData.setTriggerHook([]() { gSession->loadData(); });
    importRaw.setTriggerHook([]() { gSession->loadRawData(); });
    addExperiment.setTriggerHook([]() { gSession->createExperiment(); });

    removeExperiment.setTriggerHook([]() { gSession->removeExperiment(); });
    findPeaks.setTriggerHook([]() { new PeakFinder; });
    instrumentStates.setTriggerHook([]() { new InstrumentStates; });
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
    goniometer.setTriggerHook([]() { new GlobalOffsets(offsetMode::DETECTOR); });
    sampleGoniometer.setTriggerHook([]() { new GlobalOffsets(offsetMode::SAMPLE); });
    fromSample.setTriggerHook([](){ gGui->changeView(1); });
    behindDetector.setTriggerHook([](){ gGui->changeView(0); });
    pixelPosition.setTriggerHook([](){ gGui->cursormode(0); });
    twoTheta.setTriggerHook([](){ gGui->cursormode(1); });
    gammaNu.setTriggerHook([](){ gGui->cursormode(2); });
    dSpacing.setTriggerHook([](){ gGui->cursormode(3); });
    millerIndices.setTriggerHook([](){ gGui->cursormode(4); });
}
