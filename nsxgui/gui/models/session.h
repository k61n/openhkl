//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/models/session.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_MODELS_SESSION_H
#define NSXGUI_GUI_MODELS_SESSION_H

#include "nsxgui/gui/models/experimentmodel.h"
#include <QList>
#include <build/core/include/core/DataTypes.h>

extern class Session* gSession; //!< global handle for Session

class Session {
public:
    Session();
    ExperimentModel* selectedExperiment();
    ExperimentModel* selectExperiment(int);
    int selectedExperimentNum() { return selected; }
    ExperimentModel* experimentAt(int i) { return experiments.at(i); }
    int numExperiments() { return experiments.size(); }

    void createExperiment();
    void removeExperiment();

    void loadData();
    void loadRawData();
    void removeData();

private:
    QList<ExperimentModel*> experiments;
    int selected = -1;
};

#endif // NSXGUI_GUI_MODELS_SESSION_H
