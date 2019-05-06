
#include "gui/panels/subframe_setup.h"

SubframeSetup::SubframeSetup()
    : QTabWidget{}
{
    setTabPosition(QTabWidget::North);
    setMinimumSize(270,320);

    addTab((instrument = new TabInstrument),     "Instrument"); // 0
    addTab((data = new TabData),     "Data"); // 1
    addTab((unitcells = new TabUnitcells),     "Unit cells"); // 2
    addTab((peaks = new TabPeaks),      "Peaks"); // 3
    addTab((library = new TabPeaklibrary),    "Peaklibrary"); //4

    for (int i=0; i<4; i++)
        setTabEnabled(i, true);

    show();

//    setHook([=](const int val){
//            ASSERT(val==this->currentIndex());
//            switch (val) {
//            case 1:   gSession->params.editableRange = EditableRange::BASELINE; break;
//            case 2:   gSession->params.editableRange = EditableRange::PEAKS;    break;
//            default:  gSession->params.editableRange = EditableRange::NONE;
//            }
//        });
//    setRemake([=](){
//            setTabEnabled(1, gSession->dataset.countFiles());
//            setTabEnabled(2, gSession->dataset.countFiles());
//            if (!currentWidget()->isEnabled())
//                setCellValue(0);
//        });
}
