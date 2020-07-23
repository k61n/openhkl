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

#include "gui/models/SessionExperiment.h"

extern class Session* gSession; //!< global handle for Session

//! Class which handles the organization of the gui
class Session {
 public:
    Session();
    Session(const Session&) = delete;

    SessionExperiment* selectedExperiment() { return _experiments.at(_selectedExperiment).get(); }
    const SessionExperiment* selectedExperiment() const {
        return _experiments.at(_selectedExperiment).get(); }

    SessionExperiment* experimentAt(int i) { return _experiments.at(i).get(); }
    const SessionExperiment* experimentAt(int i) const { return _experiments.at(i).get(); }

    int selectedExperimentNum() const { return _selectedExperiment; }
    int numExperiments() const { return _experiments.size(); }
    std::vector<QString> experimentNames() const;

    void selectExperiment(int);

    bool createExperiment(QString experimentName="new_experiment",
                          QString instrumentName="unknown_instrument");
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
    std::vector<std::unique_ptr<SessionExperiment>> _experiments;
    int _selectedExperiment = -1;
    int _selectedData = -1;
    QString _loadDirectory;
};

#endif // NSX_GUI_MODELS_SESSION_H
