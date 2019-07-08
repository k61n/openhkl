//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/SessionExperiment.h
//! @brief     Defines class SessionExperiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_MODELS_SESSIONEXPERIMENT_H
#define GUI_MODELS_SESSIONEXPERIMENT_H

#include "core/experiment/Experiment.h"
#include "core/instrument/InstrumentTypes.h"
#include "core/peak/Peak3D.h"
#include "core/shape/ShapeLibrary.h"
#include "tables/crystal/UnitCell.h"
#include <QMap>

//! Controls and handles the Experiment and its Peaks and UnitCells
class SessionExperiment {
 public:
    SessionExperiment();

    nsx::sptrExperiment experiment() { return experiment_; }
    QStringList getDataNames();
    nsx::sptrDataSet getData(int index = -1);
    QList<nsx::sptrDataSet> allData();
    int getIndex(const QString&);
    void selectData(int selected) { dataIndex_ = selected; }
    void
    addPeaks(nsx::PeakList peaks, const QString& listname, const QString& uppername = QString());
    nsx::PeakList getPeaks(int upperindex = -1, int lowerindex = -1);
    nsx::PeakList getPeaks(const QString& peakListName);
    QStringList getPeakListNames(int depth = 1);
    void removePeaks(const QString& listname = QString());
    void selectPeaks(const QString& listname = QString());
    void addUnitCell(nsx::sptrUnitCell uc) { unitCells_.append(uc); }
    nsx::sptrUnitCell getUnitCell(int index = -1);
    void removeUnitCell(int index = -1);
    QStringList getUnitCellNames();
    void selectUnitCell(int select) { unitCellIndex_ = select; }
    void changeInstrument(const QString& instrumentname);
    void integratePeaks();
    void setLibrary(nsx::sptrShapeLibrary shapeLibrary) { library_ = shapeLibrary; }
    nsx::sptrShapeLibrary getLibrary() { return library_; }

 private:
    nsx::sptrExperiment experiment_;
    nsx::sptrShapeLibrary library_;
    QMap<QString, QMap<QString, nsx::PeakList>> peakLists_;
    QList<nsx::sptrUnitCell> unitCells_;
    QString selectedList_;
    int unitCellIndex_ = -1;
    int dataIndex_ = -1;
};

#endif // GUI_MODELS_SESSIONEXPERIMENT_H
