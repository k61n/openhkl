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

#include "gui/MainWin.h"
#include "gui/dialogs/HDF5ConverterDialog.h"
#include "gui/dialogs/IntegrateDialog.h"
#include "gui/dialogs/IsotopesDatabaseDialog.h"
#include "gui/dialogs/ListNameDialog.h"
#include "gui/dialogs/PeakFilterDialog.h"
#include "gui/dialogs/ShapeLibraryDialog.h"
#include "gui/frames/AutoIndexerFrame.h"
#include "gui/frames/GlobalOffsetsFrame.h"
#include "gui/frames/InstrumentStatesFrame.h"
#include "gui/frames/PeakFinderFrame.h"
#include "gui/frames/RefinerFrame.h"
#include "gui/frames/UserDefinedUnitCellIndexerFrame.h"
#include "gui/models/Session.h" //for gSession
#include "gui/panels/SubframeSetup.h"
#include "gui/panels/TabInstrument.h"
#include <QDate>
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
    findPeaks.setTriggerHook([]() { new PeakFinderFrame; });
    instrumentStates.setTriggerHook([]() { new InstrumentStatesFrame; });
    convertHDF5.setTriggerHook([]() {
        if (gSession->selectedExperimentNum() < 0)
            return;
        HDF5ConverterDialog* dlg = new HDF5ConverterDialog;
        dlg->exec();
    });
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
    goniometer.setTriggerHook([]() { new GlobalOffsetsFrame(offsetMode::DETECTOR); });
    sampleGoniometer.setTriggerHook([]() { new GlobalOffsetsFrame(offsetMode::SAMPLE); });
    isotopesDatabase.setTriggerHook([]() {
        IsotopesDatabaseDialog* iso = new IsotopesDatabaseDialog;
        iso->exec();
    });
}

void Actions::setupOptions()
{
    fromSample.setTriggerHook([]() { gGui->changeView(1); });
    behindDetector.setTriggerHook([]() { gGui->changeView(0); });
    pixelPosition.setTriggerHook([]() { gGui->cursormode(0); });
    twoTheta.setTriggerHook([]() { gGui->cursormode(1); });
    gammaNu.setTriggerHook([]() { gGui->cursormode(2); });
    dSpacing.setTriggerHook([]() { gGui->cursormode(3); });
    millerIndices.setTriggerHook([]() { gGui->cursormode(4); });
    logarithmicScale.setHook([](bool checked) {
        gGui->dockImage_->centralWidget->imageView->getScene()->setLogarithmic(checked);
    });
    showLabels.setHook([](bool checked) {
        gGui->dockImage_->centralWidget->imageView->getScene()->showPeakLabels(checked);
    });
    showAreas.setHook([](bool checked) {
        gGui->dockImage_->centralWidget->imageView->getScene()->showPeakAreas(checked);
    });
    drawPeakArea.setHook([](bool checked) {
        gGui->dockImage_->centralWidget->imageView->getScene()->drawIntegrationRegion(checked);
    });
}

void Actions::setupPeaks()
{
    peaksProperties.setTriggerHook([]() {
        SubframeSetup* properties = gGui->dockProperties_->tabsframe;
        int i = properties->indexOf(properties->peaks);
        properties->setCurrent(i);
    });
    autoIndexer.setTriggerHook([]() { new AutoIndexerFrame; });
    filterPeaks.setTriggerHook([]() { new PeakFilterDialog; });
    userDefinedIndexer.setTriggerHook([]() { new UserDefinedUnitCellIndexerFrame; });
    assignUnitCell.setTriggerHook([]() {
        if (gSession->selectedExperimentNum() < 0) {
            gLogger->log("[ERROR] No experiment selected");
            return;
        }
        if (gSession->selectedExperiment()->getPeakListNames().empty()) {
            gLogger->log("[ERROR] No peaks in selected experiment");
            return;
        }
        // gSession->selectedExperiment()->autoAssignUnitCell();
    });
    buildShapeLibrary.setTriggerHook([]() { new ShapeLibraryDialog; });
    refine.setTriggerHook([]() { new RefinerFrame; });
    normalize.setTriggerHook([]() {
        if (gSession->selectedExperimentNum() < 0)
            return;
        // gSession->selectedExperiment()->normalizeToMonitor();
    });
    integratepeaks.setTriggerHook([]() { new IntegrateDialog; });
}

void Actions::setupRest()
{
    reset.setTriggerHook([]() { gGui->resetViews(); });
    viewExperiment.setHook([](bool check) { gGui->dockExperiments_->setVisible(check); });
    viewImage.setHook([](bool check) { gGui->dockImage_->setVisible(check); });
    viewLogger.setHook([](bool check) { gGui->dockLogger_->setVisible(check); });
    viewPlotter.setHook([](bool check) { gGui->dockPlot_->setVisible(check); });
    viewProperties.setHook([](bool check) { gGui->dockProperties_->setVisible(check); });
    exportPlot.setTriggerHook([]() { gGui->exportPlot(); });
    about.setTriggerHook([]() {
        QMessageBox::about(
            gGui, "About NSXTool",
            QString("<h4>%1 version %2</h4>"
                    "<p>Copyright: Forschungszentrum Jülich GmbH %3</p>"
                    "<p>NSXTool is work in progress.<br>"
                    "It must not be used in production without consent of the developers."
                    "<br>Information on the proper form of citation will follow.<br></p>"
                    "<p>The initial authors were Laurent Chapon (ILL),"
                    " Eric Pellegrini (ILL) and Jonathan M. Fisher (FZJ JCNS MLZ).</p>"
                    "<p>Current development team is done by Janike Katter, "
                    "Alexander Schober, Joachim Wuttke (all FZJ JCNS MLZ).</p>")
                .arg(qApp->applicationName())
                .arg(qApp->applicationVersion())
                .arg(QDate::currentDate().toString("yyyy")));
    });
    helpExperiment.setTriggerHook([]() {
        QMessageBox::information(
            gGui, "Experiment Help",
            QString("<h4>Experiments</h4>"
                    "<p>Before loading your data, it is possible to create an "
                    "experiment in the <i>Start</i> menu.<br>"
                    "If you do not want to create an experiment before loading "
                    "the data, a default experiment with date and time as name "
                    "and the BioDiff 2500 as instrument will be created.</p>"));
    });
    helpData.setTriggerHook([]() {
        QMessageBox::information(
            gGui, "Data Help",
            QString("<h4>Data</h4>"
                    "<h5>Loading data</h5>"
                    "<p>You have two ways of loading data into NSXTool: <br>"
                    ">loading raw data<br>"
                    ">loading addited data<br>"
                    "For loading raw data, you can find the trigger in the menu "
                    "<i>Experiment->Data->import raw data</i>.<br>"
                    "Other data you can load with <i>Experiment->Data->load data"
                    "</i></p>"
                    "<h5>Removing data</h5>"
                    "<p>You can remove the selected data with <i>Experiment->"
                    "Data->remove data</i></p>"));
    });
}
