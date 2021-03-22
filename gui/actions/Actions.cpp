//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/actions/Actions.cpp
//! @brief     Implements class Actions
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/actions/Actions.h"

#include "gui/MainWin.h" // for gGui
#include "gui/detector_window/DetectorWindow.h"
#include "gui/models/Session.h" //for gSession
#include "gui/subframe_home/SubframeHome.h"

Actions::Actions()
{
    setupData();
    setupExperiment();
    setupPeaks();
    setupFiles();
    setupView();
    setupInstrument();
    setupOptions();
    setupRest();
}

void Actions::setupFiles()
{
    new_experiment = new QAction("New experiment");
    load_experiment = new QAction("Load experiment");
    save_experiment = new QAction("Save");
    save_all_experiment = new QAction("Save all");
    remove_experiment = new QAction("Remove experiment");
    quit = new QAction("Quit");

    connect(new_experiment, &QAction::triggered, []() { gGui->home->createNew(); });
    connect(load_experiment, &QAction::triggered, []() { gGui->home->loadFromFile(); });
    connect(save_experiment, &QAction::triggered, []() { gGui->home->saveCurrent(); });
    connect(save_all_experiment, &QAction::triggered, []() { gGui->home->saveAll(); });
    //    connect(
    //        remove_experiment, &QAction::triggered,
    //        [](){gGui->home->remo();}
    //     );
    connect(quit, &QAction::triggered, []() { gGui->close(); });
}

void Actions::setupView()
{
    detector_window = new QAction("Open detector window");

    connect(detector_window, &QAction::triggered, []() {
        gGui->detector_window->show();
        gGui->detector_window->refreshAll();
    });
}

void Actions::setupData()
{
    // loadData.setTriggerHook([]() { gSession->loadData(); });
    // removeData.setTriggerHook([]() { gSession->removeData(); });
    // importRaw.setTriggerHook([]() { gSession->loadRawData(); });
    // findPeaks.setTriggerHook([]() { new SubframeFindPeaks; });
    // instrumentStates.setTriggerHook([]() { new InstrumentStatesFrame; });
    // convertHDF5.setTriggerHook([]() {
    //     if (gSession->currentProjectNum() < 0)
    //         return;
    //     HDF5ConverterDialog* dlg = new HDF5ConverterDialog;
    //     dlg->exec();
    // });
}

void Actions::setupExperiment() { }

void Actions::setupInstrument()
{
    // goniometer.setTriggerHook([]() { new GlobalOffsetsFrame(offsetMode::DETECTOR); });
    // sampleGoniometer.setTriggerHook([]() { new GlobalOffsetsFrame(offsetMode::SAMPLE); });
    // isotopesDatabase.setTriggerHook([]() {
    //     IsotopesDatabaseDialog* iso = new IsotopesDatabaseDialog;
    //     iso->exec();
    // });
    // shapeLoadMovie.setTriggerHook([]() {
    //     if (gSession->currentProjectNum() < 0) {
    //         qWarning() << "No selected experiment";
    //         return;
    //     }

    //     new AbsorptionDialog;
    // });
}

void Actions::setupOptions()
{
    // fromSample.setTriggerHook([]() { gGui->changeView(1); });
    // behindDetector.setTriggerHook([]() { gGui->changeView(0); });
    // pixelPosition.setTriggerHook([]() { gGui->cursormode(0); });
    // twoTheta.setTriggerHook([]() { gGui->cursormode(1); });
    // gammaNu.setTriggerHook([]() { gGui->cursormode(2); });
    // dSpacing.setTriggerHook([]() { gGui->cursormode(3); });
    // millerIndices.setTriggerHook([]() { gGui->cursormode(4); });
    // logarithmicScale.setHook([](bool checked) {
    //     gGui->_experiment->getImage()->getView()->getScene()->setLogarithmic(checked);
    // });
    // showLabels.setHook([](bool checked) {
    //     gGui->_experiment->getImage()->getView()->getScene()->showPeakLabels(checked);
    // });
    // showAreas.setHook([](bool checked) {
    //     gGui->_experiment->getImage()->getView()->getScene()->showPeakAreas(checked);
    // });
    // drawPeakArea.setHook([](bool checked) {
    //     gGui->_experiment->getImage()->getView()->getScene()->drawIntegrationRegion(checked);
    // });
}

void Actions::setupPeaks()
{
    // autoIndexer.setTriggerHook([]() { new SubframeAutoIndexer; });
    // filterPeaks.setTriggerHook([]() { new PeakFilterDialog; });
    // assignUnitCell.setTriggerHook([]() {
    //     if (gSession->currentProjectNum() < 0) {
    //         gLogger->log("[ERROR] No experiment selected");
    //         return;
    //     }
    //     if (gSession->currentProject()->getPeakListNames().empty()) {
    //         gLogger->log("[ERROR] No peaks in selected experiment");
    //         return;
    //     }
    // gSession->currentProject()->autoAssignUnitCell();
    // });
    // buildShapeCollection.setTriggerHook([]() { new ShapeCollectionDialog; });
    // refine.setTriggerHook([]() { new RefinerFrame; });
    // normalize.setTriggerHook([]() {
    //     if (gSession->currentProjectNum() < 0)
    //         return;
    //     // gSession->currentProject()->normalizeToMonitor();
    // });
    // integratepeaks.setTriggerHook([]() { gSession->currentProject()->integratePeaks(); });
    // predictPeaks.setTriggerHook([]() {
    //     PredictPeaksDialog* dgl = new PredictPeaksDialog;
    //     if (!dgl->exec()) {
    //         dgl->deleteLater();
    //         return;
    //     }
    //     dgl->deleteLater();
    // });
    // statistics.setTriggerHook([]() {
    //     if (gSession->currentProjectNum() < 0)
    //         return;
    //     if (gSession->currentProject()->getUnitCellNames().empty())
    //         return;
    //     // nsx::sptrUnitCell cell = gSession->currentProject()->getUnitCell();
    //     // nsx::SpaceGroup group = cell->spaceGroup();
    //     // nsx::PeakList list = gSession->currentProject()->getUnitCell(cell);
    //     // qDebug() << "Space Group symbol: " << QString::fromStdString(group.symbol());
    //     // new MergedPeakInformationFrame(group, list);
    // });
    // correctAbsorption.setTriggerHook([]() {
    //     if (gSession->currentProjectNum() < 0)
    //         return;
    //     if (gSession->currentProject()->getUnitCellNames().empty())
    //         return;
    //     new MCAbsorptionDialog;
    // });
}

void Actions::setupRest()
{
    // reset.setTriggerHook([]() { gGui->resetViews(); });
    // exportPlot.setTriggerHook([]() { gGui->exportPlot(); });
    // about.setTriggerHook([]() {
    //     QMessageBox::about(
    //         gGui, "About NSXTool",
    //         QString("<h4>%1 version %2</h4>"
    //                 "<p>Copyright: Forschungszentrum Jülich GmbH %3</p>"
    //                 "<p>NSXTool is work in progress.<br>"
    //                 "It must not be used in production without consent of the developers."
    //                 "<br>Information on the proper form of citation will follow.<br></p>"
    //                 "<p>The initial authors were Laurent Chapon (ILL),"
    //                 " Eric Pellegrini (ILL) and Jonathan M. Fisher (FZJ JCNS MLZ).</p>"
    //                 "<p>Current development team is done by Janike Katter, "
    //                 "Alexander Schober, Joachim Wuttke (all FZJ JCNS MLZ).</p>")
    //             .arg(qApp->applicationName())
    //             .arg(qApp->applicationVersion())
    //             .arg(QDate::currentDate().toString("yyyy")));
    // });
    // helpExperiment.setTriggerHook([]() {
    //     QMessageBox::information(
    //         gGui, "Experiment Help",
    //         QString("<h4>Experiments</h4>"
    //                 "<p>Before loading your data, it is possible to create an "
    //                 "experiment in the <i>Start</i> menu.<br>"
    //                 "If you do not want to create an experiment before loading "
    //                 "the data, a default experiment with date and time as name "
    //                 "and the BioDiff 2500 as instrument will be created.</p>"));
    // });
    // helpData.setTriggerHook([]() {
    //     QMessageBox::information(
    //         gGui, "Data Help",
    //         QString("<h4>Data</h4>"
    //                 "<h5>Loading data</h5>"
    //                 "<p>You have two ways of loading data into NSXTool: <br>"
    //                 ">loading raw data<br>"
    //                 ">loading addited data<br>"
    //                 "For loading raw data, you can find the trigger in the menu "
    //                 "<i>Experiment->Data->import raw data</i>.<br>"
    //                 "Other data you can load with <i>Experiment->Data->load data"
    //                 "</i></p>"
    //                 "<h5>Removing data</h5>"
    //                 "<p>You can remove the selected data with <i>Experiment->"
    //                 "Data->remove data</i></p>"));
    // });
}
