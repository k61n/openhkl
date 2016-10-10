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

 Forshungszentrum Juelich GmbH
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

#ifndef NSXTOOL_SESSION_H_
#define NSXTOOL_SESSION_H_

#include <memory>
#include <string>
#include <vector>
#include <memory>

#include <QPoint>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>

#include "Experiment.h"
#include "ProgressHandler.h"
#include "PeakFinder.h"

using namespace SX::Instrument;

class ExperimentItem;

namespace SX
{
namespace Data
{
    class IData;
}
}

class Session : public QObject
{
    Q_OBJECT
public:
    explicit Session(QWidget *parent = 0);
    ~Session();

    void addExperiment(const std::string& experimentName, const std::string& instrumentName);
    std::vector<std::shared_ptr<SX::Data::IData>> getSelectedNumors() const;
    std::vector<std::shared_ptr<SX::Data::IData>> getSelectedNumors(ExperimentItem* item) const;

    // ExperimentItem* getExperimentItem(Experiment* exp); // no longer used?

signals:
    void plotData(std::shared_ptr<SX::Data::IData>);
    void showPeakList(std::vector<std::shared_ptr<SX::Data::IData>>);
    void inspectWidget(QWidget*);
public slots:


    void importData();
    void findPeaks(const QModelIndex& index);

    void createNewExperiment();

    void absorptionCorrection();
    void showPeaksOpenGL();
    void findSpaceGroup();
    void computeRFactors();
    void findFriedelPairs();
    void integrateCalculatedPeaks();
    void peakFitDialog();
    void incorporateCalculatedPeaks();

private:

    QStandardItemModel* _model;

    std::shared_ptr<SX::Utils::ProgressHandler> _progressHandler;
    std::shared_ptr<SX::Data::PeakFinder> _peakFinder;
};

#endif // NSXTOOL_SESSION_H_
