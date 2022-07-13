//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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

#ifndef OHKL_GUI_MODELS_SESSION_H
#define OHKL_GUI_MODELS_SESSION_H

#include "core/raw/DataKeys.h"

#include <QString>
#include <memory>
#include <vector>

class Project;
class CellComboBox;
class DataComboBox;
class PeakComboBox;
class FoundPeakComboBox;
class PredictedPeakComboBox;
class IntegratedPeakComboBox;
class ShapeComboBox;
namespace ohkl {
enum class DataFormat;
}

extern class Session* gSession; //!< global handle for Session

//! Class which handles the organization of the gui
class Session {
 public:
    Session();
    Session(const Session&) = delete;

    Project* currentProject();
    const Project* currentProject() const;
    bool hasProject() const;

    Project* experimentAt(int i);
    const Project* experimentAt(int i) const;

    bool UpdateExperimentData(unsigned int idx, QString name, QString instrument);

    int currentProjectNum() const;
    int numExperiments() const;
    std::vector<QString> experimentNames() const;

    void selectProject(int);

    void removeExperiment(unsigned int id);

    void loadData(ohkl::DataFormat format);
    bool loadRawData();
    void removeData();

    void onDataChanged();
    void onExperimentChanged();
    void onPeaksChanged();
    void onUnitCellChanged();
    void onShapesChanged();

    void loadExperimentFromFile(QString filename);

    Project* createProject(QString experimentName, QString instrumentName);
    bool addProject(std::unique_ptr<Project> project_ptr);

    std::string generateExperimentName();

 private:
    std::vector<std::unique_ptr<Project>> _projects;
    int _currentProject = -1;
    int _selectedData = -1;

    // for updating other combos
    DataComboBox* _data_combo;
    CellComboBox* _cell_combo;
    PeakComboBox* _peak_combo;
    FoundPeakComboBox* _found_peak_combo;
    PredictedPeakComboBox* _predicted_peak_combo;
    IntegratedPeakComboBox* _integrated_peak_combo;
    ShapeComboBox* _shape_combo;
};

#endif // OHKL_GUI_MODELS_SESSION_H
