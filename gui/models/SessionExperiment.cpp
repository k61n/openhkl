//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/SessionExperiment.cpp
//! @brief     Implements class SessionExperiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/models/SessionExperiment.h"

#include "core/analyse/PeakFilter.h"
#include "core/experiment/DataSet.h"
#include "core/instrument/HardwareParameters.h"
#include "core/integration/GaussianIntegrator.h"
#include "core/integration/ISigmaIntegrator.h"
#include "core/integration/MeanBackgroundIntegrator.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/integration/Profile1DIntegrator.h"
#include "core/integration/Profile3DIntegrator.h"
#include "gui/dialogs/IntegrateDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/models/Session.h"
#include <QDateTime>
#include <QDebug>
#include <QStringList>

Peaks::Peaks()
    : name_{"empty"}, type_{listtype::PREDICTED}
    , convolutionkernel_{"unknown"}, file_{"none"}
{
    nsx::PeakList liste;
    peaks_ = liste;
}

Peaks::Peaks(nsx::PeakList peaks, const QString &name, listtype type, const QString &kernel)
    : peaks_{peaks}, name_{name}, type_{type}, convolutionkernel_{kernel}, file_{"unknown"}
{
//    nsx::sptrDataSet data = peaks_[0]->data();
//    file_ = QString::fromStdString(data->filename());
}

int Peaks::valid() {
    valid_ = 0;
    for (nsx::sptrPeak3D peak : peaks_) {
        if (peak->enabled())
            valid_++;
    }
    return valid_;
}

int Peaks::notValid() {
    return numberPeaks()-valid();
}

int Peaks::numberPeaks() {
    number_ = peaks_.size();
    return number_;
}

SessionExperiment::SessionExperiment()
{
    std::string experimentName = QDateTime::currentDateTime().toString().toStdString();
    std::set<std::string> instruments = nsx::getResourcesName("instruments");
    nsx::sptrExperiment expPtr(new nsx::Experiment(experimentName, *instruments.begin()));
    experiment_ = expPtr;
}

QStringList SessionExperiment::getDataNames()
{
    std::map<std::string, nsx::sptrDataSet> datamap = experiment_->data();
    QStringList names;
    for (auto data : datamap)
        names.append(QString::fromStdString(data.first));
    return names;
}

nsx::sptrDataSet SessionExperiment::getData(int index)
{
    if (index == -1)
        index = dataIndex_;
    if (experiment_->data().empty())
        return nullptr;
    std::string selected = getDataNames().at(index).toStdString();
    return experiment_->data().at(selected);
}

int SessionExperiment::getIndex(const QString& dataname)
{
    QStringList liste = getDataNames();
    return liste.indexOf(dataname);
}

QList<nsx::sptrDataSet> SessionExperiment::allData()
{
    std::map<std::string, nsx::sptrDataSet> map = experiment_->data();
    QList<nsx::sptrDataSet> list;
    for (auto data : map)
        list.append(data.second);
    return list;
}

void SessionExperiment::addPeaks(Peaks* peaks, const QString& uppername)
{
    if (uppername.length() == 0) {
        QVector<Peaks*> inner;
        QString listname = peaks->name_;
        peaks->name_ = "all peaks";
        inner.append(std::move(peaks));
        peakLists_.insert(listname, inner);
        return;
    }
    QString upperlist = uppername;
    if (uppername.contains('/'))
        upperlist = uppername.split('/').at(0);

    peakLists_[upperlist].append(std::move(peaks));
    gSession->onPeaksChanged();
}

const Peaks* SessionExperiment::getPeaks(int upperindex, int lowerindex)
{
    if (peakLists_.empty())
        return nullptr;

    QString outername;
    if (upperindex == -1)
        return getPeaks(selectedList_);
    else
        outername = getPeakListNames(0).at(upperindex);
    if (lowerindex == -1)
        lowerindex = 0;
    return peakLists_.value(outername).at(lowerindex);
}

const Peaks* SessionExperiment::getPeaks(const QString& peakListName)
{
    QString searchedName;
    QString filteredName;
    int index = -1;
    if (!peakListName.contains("/")) {
        index = 0;
        searchedName = peakListName;
    } else {
        QStringList listnames = peakListName.split("/");
        filteredName = listnames.at(1);
        searchedName = listnames.at(0);
        index = listNamesOf(searchedName).indexOf(filteredName);
    }

    return peakLists_.value(searchedName).at(index);
}

QStringList SessionExperiment::getPeakListNames(int depth)
{
    QStringList outernames = peakLists_.keys();
    if (depth == 0)
        return outernames;
    QStringList allnames;
    for (QString outername : outernames) {
        const QVector<Peaks*> inner = peakLists_.value(outername);
        for (const Peaks* peaks : inner)
            allnames.append(outername + "/" + peaks->name_);
    }
    return allnames;
}

QStringList SessionExperiment::listNamesOf(const QString &listname)
{
    QStringList namesInSearched;
    const QVector<Peaks*> inner = peakLists_.value(listname);
    for (const Peaks* peaks : inner)
        namesInSearched.append(peaks->name_);
    return namesInSearched;
}

void SessionExperiment::removePeaks(const QString& listname)
{
    QString toremove = listname;
    if (listname.length() == 0) {
        if (selectedList_.length() == 0)
            toremove = getPeakListNames(1).at(0);
        else
            toremove = selectedList_;
    }

    if (!toremove.contains("/")) {
        peakLists_.remove(toremove);
        return;
    }

    QStringList names = toremove.split('/');
    QString outername = names.at(0);
    QString innername = names.at(1);
    int index = listNamesOf(outername).indexOf(innername);
    peakLists_[outername].removeAt(index);
}

void SessionExperiment::selectPeaks(const QString& listname)
{
    selectedList_ = listname;
}

nsx::sptrUnitCell SessionExperiment::getUnitCell(int index)
{
    if (index == -1)
        return unitCells_.at(unitCellIndex_);
    return unitCells_.at(index);
}

void SessionExperiment::removeUnitCell(int index)
{
    if (index == -1)
        unitCells_.removeAt(unitCellIndex_);
    else
        unitCells_.removeAt(index);
}

QStringList SessionExperiment::getUnitCellNames()
{
    QStringList names;
    for (nsx::sptrUnitCell uc : unitCells_)
        names.append(QString::fromStdString(uc->name()));
    return names;
}

void SessionExperiment::changeInstrument(const QString& instrumentname)
{
    if (!experiment_->data().empty())
        return;

    std::string expname = experiment_->name();
    experiment_ = std::make_shared<nsx::Experiment>(expname, instrumentname.toStdString());
}

void SessionExperiment::integratePeaks()
{
    if (peakLists_.empty()) {
        qWarning() << "No peaks to integrate";
        return;
    }

    IntegrateDialog* dialog = new IntegrateDialog;

    QMap<QString, std::function<nsx::IPeakIntegrator*()>> integratorMap;
    integratorMap["Pixel sum integrator"] = [=]() {
        return new nsx::PixelSumIntegrator(dialog->fitCenter(), dialog->fitCov());
    };
    integratorMap["Gaussian integrator"] = [=]() {
        return new nsx::GaussianIntegrator(dialog->fitCenter(), dialog->fitCov());
    };

    if (library_) {
        integratorMap["3d profile integrator"] = [=]() {
            return new nsx::Profile3DIntegrator(
                library_, dialog->radius(), dialog->numberOfFrames(), false);
        };
        integratorMap["I/Sigma integrator"] = [=]() {
            return new nsx::ISigmaIntegrator(library_, dialog->radius(), dialog->numberOfFrames());
        };
        integratorMap["1d profile integrator"] = [=]() {
            return new nsx::Profile1DIntegrator(
                library_, dialog->radius(), dialog->numberOfFrames());
        };
    }

    dialog->setIntegrators(integratorMap.keys());
    dialog->show();

    if (!dialog->exec())
        return;

    const double dmin = dialog->minimumD();
    const double dmax = dialog->maximumD();
    QList<nsx::sptrDataSet> numors = allData();

    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView view(nullptr);
    view.watch(handler);

    nsx::PeakFilter filter;
    nsx::PeakList peaks = filter.dRange(getPeaks()->peaks_, dmin, dmax);

    for (nsx::sptrDataSet numor : numors) {
        qDebug() << "Integrationg " << peaks.size() << " peaks";
        std::unique_ptr<nsx::IPeakIntegrator> integrator(integratorMap[dialog->integrator()]());
        integrator->setHandler(handler);
        if (!library_) {
            integrator->integrate(
                peaks, numor, dialog->peakScale(), dialog->backgroundBegin(),
                dialog->backgroundScale());
        } else {
            integrator->integrate(
                peaks, numor, library_->peakScale(), library_->bkgBegin(), library_->bkgEnd());
        }
    }

    qDebug() << "Done reintegrating peaks";
    dialog->deleteLater();
}
