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

#ifndef NSXTOOL_SESSIONMODEL_H_
#define NSXTOOL_SESSIONMODEL_H_

#include <memory>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include <QPoint>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>

#include <nsxlib/instrument/Experiment.h>
#include <nsxlib/utils/ProgressHandler.h>
#include <nsxlib/data/PeakFinder.h>
#include <nsxlib/utils/Types.h>

using namespace nsx::Instrument;

class ExperimentItem;

namespace nsx {
namespace Data {
    class DataSet;
}
}

using nsx::Crystal::sptrUnitCell;
using nsx::Crystal::sptrPeak3D;


class SessionModel : public QStandardItemModel {
    Q_OBJECT
public:
    using sptrUnitCell = nsx::Crystal::sptrUnitCell;
    using sptrPeak3D = std::shared_ptr<nsx::Crystal::Peak3D>;

    explicit SessionModel();
    ~SessionModel();

    std::shared_ptr<nsx::Instrument::Experiment> addExperiment(const std::string& experimentName, const std::string& instrumentName);
    std::vector<std::shared_ptr<nsx::Data::DataSet>> getSelectedNumors() const;
    std::vector<std::shared_ptr<nsx::Data::DataSet>> getSelectedNumors(ExperimentItem* item) const;

    //! Convert session into JSON object
    QJsonObject toJsonObject();
    void fromJsonObject(const QJsonObject& obj);

    void setFilename(QString name);
    QString getFilename();

    void setColorMap(const std::string& name);
    std::string getColorMap() const;

    void writeLog();
    bool writeNewShellX(std::string filename, const std::vector<sptrPeak3D>& peaks);
    bool writeStatistics(std::string filename,
                         const std::vector<nsx::Crystal::sptrPeak3D> &peaks,
                         double dmin, double dmax, unsigned int num_shells, bool friedel);

    bool writeXDS(std::string filename, const std::vector<sptrPeak3D>& peaks, bool merge, bool friedel);

    void fitAllPeaks();
    void autoAssignUnitCell();

signals:
    void plotData(std::shared_ptr<nsx::Data::DataSet>);
    void inspectWidget(QWidget*);
    void updatePeaks();
    void updateCellParameters(sptrUnitCell);

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
    std::shared_ptr<nsx::Utils::ProgressHandler> _progressHandler;
    std::shared_ptr<nsx::Data::PeakFinder> _peakFinder;
    std::string _colormap;

    //std::map<std::string, std::shared_ptr<nsx::Instrument::Experiment>> _experiments;
};

#endif // NSXTOOL_SESSIONMODEL_H_
