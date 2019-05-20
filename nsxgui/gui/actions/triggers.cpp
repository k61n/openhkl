#include "nsxgui/gui/actions/triggers.h"
#include "nsxgui/gui/mainwin.h"
#include "nsxgui/gui/models/session.h" //for gSession
#include "nsxgui/gui/panels/tab_instrument.h"
#include "nsxgui/gui/panels/subframe_setup.h"
#include "nsxgui/gui/dialogs/open_files.h"
#include "nsxgui/gui/frames/instrumentstates.h"
#include "nsxgui/gui/frames/autoindexer.h"
#include "nsxgui/gui/frames/peakfinder.h"

#include <QInputDialog>
#include <QDesktopServices>

Triggers::Triggers()
{
    reset.setTriggerHook([](){ gGui->resetViews(); });
    quit.setTriggerHook([](){ gGui->deleteLater(); });
    monochromaticSourceProperties.setTriggerHook([this](){
        SubframeSetup* properties = gGui->dockProperties_->tabsframe;
        TabInstrument* tab = properties->instrument;
        int i = tab->indexOf(tab->monoSource);
        properties->setCurrent(0);
        tab->setCurrent(i);
    });
    shapeProperties.setTriggerHook([](){
                         SubframeSetup* properties = gGui->dockProperties_->tabsframe;
                         TabInstrument* tab = properties->instrument;
                         int i =tab->indexOf(tab->sample);
                         properties->setCurrent(0);
                         tab->setCurrent(i);
                     });
    sampleProperties.setTriggerHook([](){
                         SubframeSetup* properties = gGui->dockProperties_->tabsframe;
                        TabInstrument* tab = properties->instrument;
                         int i = tab->indexOf(tab->sample);
                         properties->setCurrent(0);
                        tab->setCurrent(i);
                     });
    detectorProperties.setTriggerHook([](){
                         SubframeSetup* properties = gGui->dockProperties_->tabsframe;
                         TabInstrument* tab = properties->instrument;
                         int i = tab->indexOf(tab->detector);
                         properties->setCurrent(0);
                         tab->setCurrent(i);
                     });
    peaksProperties.setTriggerHook([](){
                         SubframeSetup* properties = gGui->dockProperties_->tabsframe;
                         int i = properties->indexOf(properties->peaks);
                         properties->setCurrent(i);
                     });
    dataProperties.setTriggerHook([](){
                         SubframeSetup* properties = gGui->dockProperties_->tabsframe;
                         int i = properties->indexOf(properties->data);
                         properties->setCurrent(i);
                     });
    loadData.setTriggerHook([](){ gSession->loadData(); });
    importRaw.setTriggerHook([](){ gSession->loadRawData(); });
    addExperiment.setTriggerHook([](){ gSession->createExperiment(); });

    removeExperiment.setTriggerHook([](){
        gSession->removeExperiment();
    });
    findPeaks.setTriggerHook([](){
//        nsx::DataList data;
//        if (!data.isEmpty()) {
//        ExperimentItem* experiment_item = nullptr;
//        FramePeakFinder* frame = FramePeakFinder::create(experiment_item, data);
//        frame->show(); }

        new PeakFinder;
        gLogger->log("[WARNING] not yet available");
    });
    instrumentStates.setTriggerHook([](){ new InstrumentStates; });
    autoIndexer.setTriggerHook([](){ new AutoIndexer; });
}
