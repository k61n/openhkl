/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef NSXQT_SESSIONMODEL_H
#define NSXQT_SESSIONMODEL_H

#include <memory>
#include <string>
#include <vector>

#include <QPoint>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/InstrumentTypes.h>
#include <nsxlib/UtilsTypes.h>

class ExperimentItem;

class SessionModel : public QStandardItemModel {
    Q_OBJECT
public:
    explicit SessionModel();
    ~SessionModel();

    nsx::sptrExperiment addExperiment(const std::string& experimentName, const std::string& instrumentName);
    nsx::DataList getSelectedNumors() const;
    nsx::DataList getSelectedNumors(ExperimentItem* item) const;

    //! Convert session into JSON object
    QJsonObject toJsonObject();
    void fromJsonObject(const QJsonObject& obj);

    void setFilename(QString name);
    QString getFilename();

    void setColorMap(const std::string& name);
    std::string getColorMap() const;

    void writeLog();
    bool writeNewShellX(std::string filename, const nsx::PeakList& peaks);
    bool writeStatistics(std::string filename,
                         const nsx::PeakList &peaks,
                         double dmin, double dmax, unsigned int num_shells, bool friedel);

    bool writeXDS(std::string filename, const nsx::PeakList& peaks, bool merge, bool friedel);

    void fitAllPeaks();
    void autoAssignUnitCell();

signals:
    void plotData(nsx::sptrDataSet);
    void inspectWidget(QWidget*);
    void updatePeaks();
    void updateCellParameters(nsx::sptrUnitCell);

public slots:

    void importData();
    void findPeaks(const QModelIndex& index);

    void createNewExperiment();
    void absorptionCorrection();
    void showPeaksOpenGL();
    void findSpaceGroup();
    void computeRFactors();
    void findFriedelPairs();
    void peakFitDialog();
    void incorporateCalculatedPeaks();
    void applyResolutionCutoff(double dmin, double dmax);
    void onItemChanged(QStandardItem* item);

private:
    //! Filename for the save/load feature
    QString _filename;
    nsx::sptrProgressHandler _progressHandler;
    nsx::sptrPeakFinder _peakFinder;
    std::string _colormap;
};

#endif // NSXQT_SESSIONMODEL_H
