//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/Session.h
//! @brief     Defines class Session
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_MODELS_SESSION_H
#define GUI_MODELS_SESSION_H

#include "gui/models/SessionExperiment.h"

extern class Session* gSession; //!< global handle for Session

//! Class which handles the organization of the gui
class Session {
 public:
    Session();

    SessionExperiment* selectedExperiment();
    void selectExperiment(int);
    int selectedExperimentNum() { return selectedExperiment_; }
    SessionExperiment* experimentAt(int i) { return _experiments.at(i); }
    int numExperiments() { return _experiments.size(); }
    QList<QString> experimentNames() const;

    void createExperiment(QString experimentName);
    void createExperiment(QString experimentName, QString instrumentName);
    void createDefaultExperiment();
    void removeExperiment();

    void loadData();
    void loadRawData();
    void removeData();

    void onDataChanged();
    void onExperimentChanged();
    void onPeaksChanged();

 private:
    QList<SessionExperiment*> _experiments;
    int selectedExperiment_ = -1;
    int selectedData = -1;
    QString loadDirectory;
};

#endif // GUI_MODELS_SESSION_H
