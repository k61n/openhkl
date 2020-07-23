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

#ifndef NSX_GUI_MODELS_SESSION_H
#define NSX_GUI_MODELS_SESSION_H

#include "core/experiment/Experiment.h"

class Project;

extern class Session* gSession; //!< global handle for Session

//! Class which handles the organization of the gui
class Session {
 public:
    Session();
    Session(const Session&) = delete;

    Project* currentProject();
    const Project* currentProject() const;

    Project* experimentAt(int i);
    const Project* experimentAt(int i) const;

    int currentProjectNum() const;
    int numExperiments() const;
    std::vector<QString> experimentNames() const;

    void selectExperiment(int);

    bool createExperiment(
        QString experimentName = "new_experiment", QString instrumentName = "unknown_instrument");
    void removeExperiment();

    void loadData();
    void loadRawData();
    void removeData();

    void onDataChanged();
    void onExperimentChanged();
    void onPeaksChanged();
    void onUnitCellChanged();

    void loadExperimentFromFile(QString filename);

 private:
    std::vector<std::unique_ptr<Project>> _projects;
    int _currentProject = -1;
    int _selectedData = -1;
    QString _loadDirectory;
};

#endif // NSX_GUI_MODELS_SESSION_H
