#include "gui/actions/triggers.h"
#include "gui/mainwindow.h"
#include "gui/panels/tab_instrument.h"
#include "gui/panels/subframe_setup.h"
#include <QDesktopServices>

Triggers::Triggers()
{
    QAction::connect(&reset, &QAction::triggered, gGui, &MainWindow::resetViews);
    QAction::connect(&quit, &QAction::triggered, []()->void { exit(0);});
    QAction::connect(&monochromaticSourceProperties, &QAction::triggered,
                     []()->void {
                         SubframeSetup* properties = gGui->dockProperties_->tabsframe;
                        TabInstrument* tab = properties->instrument;
                         int i = tab->indexOf(tab->monoSource);
                         properties->setCurrentIndex(0);
                         tab->setCurrentIndex(i);
                     });
    QAction::connect(&shapeProperties, &QAction::triggered, []()->void{
                         SubframeSetup* properties = gGui->dockProperties_->tabsframe;
                         TabInstrument* tab = properties->instrument;
                         int i =tab->indexOf(tab->sample);
                         properties->setCurrentIndex(0);
                         tab->setCurrentIndex(i);
                     });
    QAction::connect(&sampleProperties, &QAction::triggered, []()->void{
                         SubframeSetup* properties = gGui->dockProperties_->tabsframe;
                        TabInstrument* tab = properties->instrument;
                         int i = tab->indexOf(tab->sample);
                         properties->setCurrentIndex(0);
                        tab->setCurrentIndex(i);
                     });
    QAction::connect(&detectorProperties, &QAction::triggered, []()->void{
                         SubframeSetup* properties = gGui->dockProperties_->tabsframe;
                         TabInstrument* tab = properties->instrument;
                         int i = tab->indexOf(tab->detector);
                         properties->setCurrentIndex(0);
                         tab->setCurrentIndex(i);
                     });
    QAction::connect(&peaksPropeerties, &QAction::triggered, []()->void{
                         SubframeSetup* properties = gGui->dockProperties_->tabsframe;
                         int i = properties->indexOf(properties->peaks);
                         properties->setCurrentIndex(i);
                     });
    QAction::connect(&dataProperties, &QAction::triggered, []()->void{
                         SubframeSetup* properties = gGui->dockProperties_->tabsframe;
                         int i = properties->indexOf(properties->data);
                         properties->setCurrentIndex(i);
                     });
}
