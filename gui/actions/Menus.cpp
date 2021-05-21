//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/actions/Menus.cpp
//! @brief     Implements class Menus
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/actions/Menus.h"

#include "gui/MainWin.h"
#include "gui/actions/Actions.h"

#include <QAction>
#include <QMenu>

//! Initialize the menu bar.
Menus::Menus(QMenuBar* menu_bar) : _menu_bar{menu_bar}
{
    Actions* actions = gGui->triggers;

    _menu_bar->setNativeMenuBar(true);

    _expt_menu = _menu_bar->addMenu("Experiment");
    // _edit_menu = _menu_bar->addMenu("Edit");
    _view_menu = _menu_bar->addMenu("View");
    _data_menu = _menu_bar->addMenu("Data");
    _peaks_menu = _menu_bar->addMenu("Peaks");
    _cells_menu = _menu_bar->addMenu("Cells");
    _help_menu = _menu_bar->addMenu("Help");

    _expt_menu->addAction(actions->new_experiment);
    _expt_menu->addAction(actions->load_experiment);
    _expt_menu->addSeparator();
    _expt_menu->addAction(actions->save_experiment);
    _expt_menu->addAction(actions->save_all_experiment);
    _expt_menu->addSeparator();
    _expt_menu->addAction(actions->remove_experiment);
    _expt_menu->addAction(actions->quit);

    _view_menu->addAction(actions->detector_window);

    QMenu* _data_sub = _data_menu->addMenu("Add data set");
    _data_sub->addAction(actions->add_raw);
    _data_sub->addAction(actions->add_hdf5);
    _data_sub->addAction(actions->add_nexus);
    _data_menu->addAction(actions->remove_data);

    // _peaks_menu->addAction(actions->add_peaks);
    _peaks_menu->addAction(actions->remove_peaks);

    _cells_menu->addAction(actions->add_cell);
    _cells_menu->addAction(actions->remove_cell);

    _help_menu->addAction(actions->about);


    // actionsToMenu(
    //     "&Start",
    //     {&actions->addExperiment, &actions->removeExperiment, separator(), &actions->loadData,
    //      &actions->importRaw, &actions->removeData, separator(), &actions->quit});

    // QMenu* detector = new QMenu {"&Detector"};
    // detector->addActions({&actions->detectorProperties, &actions->goniometer});
    // QMenu* sample = new QMenu {"&Sample"};
    // sample->addActions(
    //     {&actions->sampleProperties, &actions->sampleGoniometer, &actions->isotopesDatabase});
    // sample->addSeparator()->setText("Shape");
    // sample->addActions({&actions->shapeProperties, &actions->shapeLoadMovie});
    // QMenu* instrument = new QMenu {"&Instrument"};
    // instrument->addAction(&actions->instrumentStates);
    // instrument->addMenu(detector);
    // instrument->addMenu(sample);
    // instrument->addSeparator()->setText("Monochromatic source");
    // instrument->addAction(&actions->monochromaticSourceProperties);

    // QMenu* data = new QMenu {"&Data"};
    // data->addActions({&actions->loadData, &actions->importRaw, &actions->removeData,
    //                   &actions->dataProperties, &actions->convertHDF5});
    // QMenu* indexing = new QMenu {"&indexing"};
    // indexing->addActions(
    //     {&actions->autoIndexer, &actions->userDefinedIndexer, &actions->assignUnitCell});
    // QMenu* peaks = new QMenu {"&Peaks"};
    // peaks->addActions({&actions->findPeaks, &actions->filterPeaks});
    // peaks->addMenu(indexing);
    // peaks->addActions({&actions->refine, &actions->buildShapeCollection,
    // &actions->integratepeaks,
    //                    &actions->normalize, &actions->correctAbsorption, &actions->predictPeaks,
    //                    &actions->statistics, &actions->show3d, &actions->peaksProperties});
    // experiment_ = mbar_->addMenu("&Experiment");
    // experiment_->addMenu(data);
    // experiment_->addMenu(peaks);
    // experiment_->addMenu(instrument);
    // experiment_->addSeparator()->setText("unit cells");
    // experiment_->addAction(&actions->removeUnusedUnitCells);


    // actionsToMenu("&Export", {&actions->exportPlot});

    // options_ = new QMenu {"&Detector View"};
    // QMenu* cursorMode = new QMenu {"&Cursor mode"};
    // cursorMode->addActions({&actions->pixelPosition, &actions->gammaNu, &actions->twoTheta,
    //                         &actions->dSpacing, &actions->millerIndices});
    // QMenu* setView = new QMenu {"&Set Image View"};
    // setView->addActions(
    //     {&actions->fromSample, &actions->behindDetector, &actions->logarithmicScale});
    // QMenu* peakMenu = new QMenu {"&Peak"};
    // peakMenu->addActions({&actions->showLabels, &actions->showAreas, &actions->drawPeakArea});
    // options_->addMenu(cursorMode);
    // options_->addMenu(setView);
    // options_->addMenu(peakMenu);

    // view_ = new QMenu("&Main Window");
    // view_->addActions({&actions->reset, separator(), &actions->viewExperiment,
    // &actions->viewImage,
    //                    &actions->viewLogger, &actions->viewPlotter, &actions->viewProperties});

    // QMenu* viewsMenu = mbar_->addMenu("&Views");
    // viewsMenu->addMenu(options_);
    // viewsMenu->addMenu(view_);

    // actionsToMenu(
    //     "&Help",
    //     {&actions->about, &actions->helpExperiment, &actions->helpData, &actions->helpPeakFinder,
    //      &actions->helpPeakFilter});
}

QAction* Menus::separator() const
{
    QAction* ret = new QAction{_menu_bar};
    ret->setSeparator(true);
    return ret;
}

QMenu* Menus::actionsToMenu(const char* menuName, QList<QAction*> actions)
{
    QMenu* menu = new QMenu{menuName};
    _menu_bar->addMenu(menu);
    menu->addActions(actions);
    QString prefix = QString("%1: ").arg(menu->title().remove('&'));
    for (auto action : actions)
        action->setToolTip(prefix + action->toolTip());
    return menu;
}
