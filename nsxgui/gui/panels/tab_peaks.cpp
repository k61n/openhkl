
#include "nsxgui/gui/panels/tab_peaks.h"
#include "nsxgui/gui/models/experimentmodel.h"
#include "nsxgui/gui/models/session.h"
#include "nsxgui/qcr/engine/logger.h"
#include <QHBoxLayout>
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! @class TabPeaks

TabPeaks::TabPeaks()
    : QcrWidget{"peaks"}
{
    auto* layout = new QHBoxLayout(this);

    view = new PeaksTableView;
    layout->addWidget(view);

    setRemake([this](){
        if (gSession->selectedExperimentNum() >= 0) {
            if (!gSession->selectedExperiment()->peaks()->allPeaks().empty()) {
                PeaksTableModel* model =
                        new PeaksTableModel("tablePeaks",
                                            gSession->selectedExperiment()->experiment(),
                                            gSession->selectedExperiment()->peaks()->allPeaks());
                view->setModel(model);
            } else {
                PeaksTableModel* model = new PeaksTableModel("emptyTable",
                                                             gSession->selectedExperiment()->experiment());
                view->setModel(model);
            }
        }
    });
}

