
#include "gui/actions/menus.h"
#include "gui/view/toggles.h"
#include "gui/actions/triggers.h"
#include "gui/mainwindow.h"
#include <QAction>
#include <QMenu>

//! Initialize the menu bar.
Menus::Menus(QMenuBar* mbar)
    : mbar_{mbar}
{
    Triggers* triggers = gGui->triggers;
    Toggles* toggles = gGui->toggles;
    mbar->setNativeMenuBar(true);

    actionsToMenu("&File",
    { &triggers->addExperiment,
      &triggers->removeExperiment,
      separator(),
      &triggers->quit });

    QMenu* detector = new QMenu{"&Detector"};
    detector->addActions({ &triggers->detectorProperties,
                           &triggers->goniometer});
    QMenu* sample = new QMenu{"&Sample"};
    sample->addActions({ &triggers->sampleProperties,
                       &triggers->sampleGoniometer,
                         &triggers->isotopesDatabase});
    sample->addSeparator()->setText("Shape");
    sample->addActions({&triggers->shapeProperties,
                        &triggers->shapeLoadMovie});
    QMenu* instrument = new QMenu{"&Instrument"};
    instrument->addMenu(detector);
    instrument->addMenu(sample);
    instrument->addSeparator()->setText("Monochromatic source");
    instrument->addAction(&triggers->monochromaticSourceProperties);

    QMenu* data = new QMenu{"&Data"};
    data->addActions({ &triggers->loadData,
                     &triggers->removeData,
                     &triggers->dataProperties,
                     &triggers->convertHDF5,
                     &triggers->importRaw,
                     &triggers->instrumentStates,
                     &triggers->findPeaks});
    QMenu* indexing = new QMenu{"&indexing"};
    indexing->addActions({&triggers->autoIndexer, &triggers->userDefinedIndexer,
                         &triggers->assignUnitCell});
    QMenu* peaks = new QMenu{"&Peaks"};
    peaks->addAction(&triggers->filterPeaks);
    peaks->addMenu(indexing);
    peaks->addActions({&triggers->buildShapeLibrary,
                      &triggers->integratepeaks,
                      &triggers->normalize,
                      &triggers->correctAbsorption,
                      &triggers->show3d,
                      &triggers->peaksPropeerties});
    experiment_ = mbar_->addMenu("&Experiments");
    experiment_->addMenu(instrument);
    experiment_->addMenu(data);
    experiment_->addMenu(peaks);
    experiment_->addSeparator()->setText("reference peak library");
    experiment_->addAction(&triggers->predictPeaks);
    experiment_->addSeparator()->setText("unit cells");
    experiment_->addAction(&triggers->removeUnusedUnitCells);


    actionsToMenu("&Export", {&triggers->exportPlot});

    options_ = mbar_->addMenu("&Options");
    QMenu* cursorMode = new QMenu{"&Cursor mode"};
    cursorMode->addActions({    &toggles->pixelPosition,
                                &toggles->gammaNu,
                                &toggles->twoTheta,
                                &toggles->dSpacing,
                                &toggles->millerIndices
                           });
    QMenu* setView = new QMenu{"&Set View"};
    setView->addActions({
                            &toggles->fromSample,
                            &toggles->behindDetector,
                            &toggles->logarithmicScale
                        });
    QMenu* peakMenu = new QMenu{"&Peak"};
    peakMenu->addActions({
                             &toggles->showLabels,
                             &toggles->showAreas,
                             &toggles->drawPeakArea
                         });
    options_->addMenu(cursorMode);
    options_->addMenu(setView);
    options_->addMenu(peakMenu);

    actionsToMenu("&View",
    {&triggers->reset});

    actionsToMenu("&Help",
    {&triggers->about});
}

QAction* Menus::separator() const
{
    QAction* ret = new QAction{mbar_};
    ret->setSeparator(true);
    return ret;
}

QMenu* Menus::actionsToMenu(const char* menuName, QList<QAction*> actions)
{
    QMenu* menu = new QMenu { menuName };
    mbar_->addMenu(menu);
    menu->addActions(actions);
    QString prefix = QString("%1: ").arg(menu->title().remove('&'));
    for (auto action : actions)
        action->setToolTip(prefix + action->toolTip());
    return menu;
}
