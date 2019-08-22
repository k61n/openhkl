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
#include "gui/MainWin.h"
#include "gui/items/PeakCollectionItem.h"

#include <QDateTime>
#include <QDebug>
#include <QStringList>
#include <QStandardItem>

SessionExperiment::SessionExperiment()
{
    std::string experimentName = QDateTime::currentDateTime().toString().toStdString();
    std::set<std::string> instruments = nsx::getResourcesName("instruments");
    nsx::sptrExperiment expPtr(new nsx::Experiment(experimentName, *instruments.begin()));
    _experiment = expPtr;
}

SessionExperiment::SessionExperiment(QString name, QString instrument)
{
    nsx::sptrExperiment expPtr(new nsx::Experiment(name.toStdString(), instrument.toStdString()));
    _experiment = expPtr;
}

QStringList SessionExperiment::getDataNames()
{
    std::map<std::string, nsx::sptrDataSet> datamap = _experiment->data();
    QStringList names;
    for (auto data : datamap)
        names.append(QString::fromStdString(data.first));
    return names;
}

nsx::sptrDataSet SessionExperiment::getData(int index)
{
    if (index == -1)
        index = dataIndex_;
    if (_experiment->data().empty())
        return nullptr;
    std::string selected = getDataNames().at(index).toStdString();
    return _experiment->data().at(selected);
}

int SessionExperiment::getIndex(const QString& dataname)
{
    QStringList liste = getDataNames();
    return liste.indexOf(dataname);
}

QList<nsx::sptrDataSet> SessionExperiment::allData()
{
    std::map<std::string, nsx::sptrDataSet> map = _experiment->data();
    QList<nsx::sptrDataSet> list;
    for (auto data : map)
        list.append(data.second);
    return list;
}

// void SessionExperiment::addPeaks(Peaks* peaks, const QString& uppername)
// {
//     // if (uppername.length() == 0) {
//     //     QVector<Peaks*> inner;
//     //     QString listname = peaks->name_;
//     //     peaks->name_ = "all peaks";
//     //     inner.append(std::move(peaks));
//     //     peakLists_.insert(listname, inner);
//     //     gSession->onPeaksChanged();
//     //     return;
//     // }
//     // QString upperlist = uppername;
//     // if (uppername.contains('/'))
//     //     upperlist = uppername.split('/').at(0);

//     // peakLists_[upperlist].append(std::move(peaks));

//     gSession->onPeaksChanged();
// }

QStringList SessionExperiment::getPeakListNames(int depth)
{
    std::vector<std::string*> names = _experiment->getCollectionNames();
    QStringList q_names;

    for (std::string* name :names){
        q_names<<QString::fromStdString(*name);
    }

    return q_names;
}

void SessionExperiment::setSelected(std::string name)
{
    _selected = peakModel(QString::fromStdString(name));
}

PeakCollectionModel* SessionExperiment::selected()
{
    return _selected;
}

void SessionExperiment::generatePeakModel(const QString& peakListName)
{
    if( !_experiment->hasPeakCollection(peakListName.toStdString())){
        return;
    }
    nsx::PeakCollection* peak_collection = _experiment->getPeakCollection(
        peakListName.toStdString());

    PeakCollectionItem* peak_collection_item = new PeakCollectionItem(peak_collection);
    PeakCollectionModel* peak_collection_model = new PeakCollectionModel();
    peak_collection_model->setRoot(peak_collection_item);
    _peak_models.append(peak_collection_model);

    generatePeakListModel();
}

PeakCollectionModel* SessionExperiment::peakModel(const QString& name)
{
    std::string std_name = name.toStdString();
    for (int i = 0; i < _peak_models.size(); ++i) {
        if (*_peak_models.at(i)->name() == std_name)
            return _peak_models.at(i);
    }

    return nullptr;
}
   
void SessionExperiment::generatePeakListModel()
{
    _peak_list_model.clear();
    std::vector<std::string*> names = _experiment->getCollectionNames();
    for (std::string* name :names){
        QStandardItem* item = new QStandardItem(QString::fromStdString(*name));
        _peak_list_model.appendRow(item);
    }

    onPeaksChanged();
}

std::vector<nsx::Peak3D*>* SessionExperiment::getPeaks(
    const QString& peakListName, 
    int upperindex, 
    int lowerindex){

    if( !_experiment->hasPeakCollection(peakListName.toStdString())){
        return nullptr;
    }

    nsx::PeakCollection* peakCollection = _experiment->getPeakCollection(
        peakListName.toStdString());

    std::vector<nsx::Peak3D*>* peaks = peakCollection->getPeakList();

    // if (upperindex == -1)
    //     upperindex = 0;
    // if (lowerindex == -1)
    //     lowerindex = 0;

    return peaks;
}


// std::vector<nsx::Peak3D*> SessionExperiment::getPeaks(const QString& peakListName)
// {

//     QString searchedName;
//     QString filteredName;
//     int index = -1;
//     if (!peakListName.contains("/")) {
//         index = 0;
//         searchedName = peakListName;
//     } else {
//         QStringList listnames = peakListName.split("/");
//         filteredName = listnames.at(1);
//         searchedName = listnames.at(0);
//         index = listNamesOf(searchedName).indexOf(filteredName);
//     }

//     return peakLists_.value(searchedName).at(index);
// }

// std::vector<nsx::Peak3D*> SessionExperiment::getPeakList(nsx::sptrUnitCell cell)
// {
//     nsx::PeakList ret;
//     for (QVector<Peaks*> vec : peakLists_) {
//         for (Peaks* peaks : vec) {
//             if (peaks->type_ != listtype::FILTERED) {
//                 nsx::PeakList list = peaks->peaks_;
//                 for (nsx::sptrPeak3D p : list) {
//                     if (p->unitCell() && p->unitCell()->name() == cell->name()){
//                             ret.push_back(p);
//                     }
//                 }
//             }
//         }
//     }
//     return ret;
// }

// std::vector<nsx::Peak3D*> SessionExperiment::getPeakList(nsx::sptrDataSet data)
// {
//     nsx::PeakList ret;
//     for (QVector<Peaks*> vec : peakLists_) {
//         for (Peaks* peaks : vec) {
//             if (peaks->type_ != listtype::FILTERED
//                     && peaks->file_ == QString::fromStdString(data->filename())) {
//                 for (nsx::sptrPeak3D p : peaks->peaks_) {
//                     ret.push_back(p);
//                 }
//             }
//         }
//     }
//     return ret;
// }



// QStringList SessionExperiment::listNamesOf(const QString &listname)
// {
//     QStringList namesInSearched;
//     const QVector<Peaks*> inner = peakLists_.value(listname);
//     for (const Peaks* peaks : inner)
//         namesInSearched.append(peaks->name_);
//     return namesInSearched;
// }

// void SessionExperiment::removePeaks(const QString& listname)
// {
//     QString toremove = listname;
//     if (listname.length() == 0) {
//         if (selectedList_.length() == 0)
//             toremove = getPeakListNames(1).at(0);
//         else
//             toremove = selectedList_;
//     }

//     if (!toremove.contains("/")) {
//         peakLists_.remove(toremove);
//         return;
//     }

//     QStringList names = toremove.split('/');
//     QString outername = names.at(0);
//     QString innername = names.at(1);
//     int index = listNamesOf(outername).indexOf(innername);
//     peakLists_[outername].removeAt(index);
// }

// void SessionExperiment::selectPeaks(const QString& listname)
// {
//     selectedList_ = listname;
// }

nsx::sptrUnitCell SessionExperiment::getUnitCell(int index)
{
    if (index == -1) {
        if (unitCellIndex_ == -1)
            index = 0;
        else
            index = unitCellIndex_;
    }

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
    if (!_experiment->data().empty())
        return;

    std::string expname = _experiment->name();
    _experiment = std::make_shared<nsx::Experiment>(expname, instrumentname.toStdString());
}

void SessionExperiment::integratePeaks()
{
    // if (peakLists_.empty()) {
    //     qWarning() << "No peaks to integrate";
    //     return;
    // }

    // IntegrateDialog* dialog = new IntegrateDialog;

    // QMap<QString, std::function<nsx::IPeakIntegrator*()>> integratorMap;
    // integratorMap["Pixel sum integrator"] = [=]() {
    //     return new nsx::PixelSumIntegrator(dialog->fitCenter(), dialog->fitCov());
    // };
    // integratorMap["Gaussian integrator"] = [=]() {
    //     return new nsx::GaussianIntegrator(dialog->fitCenter(), dialog->fitCov());
    // };

    // if (_library) {
    //     integratorMap["3d profile integrator"] = [=]() {
    //         return new nsx::Profile3DIntegrator(
    //             _library, dialog->radius(), dialog->numberOfFrames(), false);
    //     };
    //     integratorMap["I/Sigma integrator"] = [=]() {
    //         return new nsx::ISigmaIntegrator(_library, dialog->radius(), dialog->numberOfFrames());
    //     };
    //     integratorMap["1d profile integrator"] = [=]() {
    //         return new nsx::Profile1DIntegrator(
    //             _library, dialog->radius(), dialog->numberOfFrames());
    //     };
    // }

    // dialog->setIntegrators(integratorMap.keys());
    // dialog->show();

    // if (!dialog->exec()) {
    //     dialog->deleteLater();
    //     return;
    // }

    // const double dmin = dialog->minimumD();
    // const double dmax = dialog->maximumD();
    // QList<nsx::sptrDataSet> numors = allData();

    // nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    // ProgressView view(nullptr);
    // view.watch(handler);

    // nsx::PeakFilter filter;
    // nsx::PeakList peaks = filter.dRange(getPeaks()->peaks_, dmin, dmax);

    // for (nsx::sptrDataSet numor : numors) {
    //     qDebug() << "Integrationg " << peaks.size() << " peaks";
    //     std::unique_ptr<nsx::IPeakIntegrator> integrator(integratorMap[dialog->integrator()]());
    //     integrator->setHandler(handler);
    //     if (!_library) {
    //         integrator->integrate(
    //             peaks, numor, dialog->peakScale(), dialog->backgroundBegin(),
    //             dialog->backgroundScale());
    //     } else {
    //         integrator->integrate(
    //             peaks, numor, _library->peakScale(), _library->bkgBegin(), _library->bkgEnd());
    //     }
    // }

    // qDebug() << "Done reintegrating peaks";
    // dialog->deleteLater();
}

void SessionExperiment::onPeaksChanged()
{
    gGui->onPeaksChanged();
}

bool SessionExperiment::saveToFile(QString path)
{
    bool success = experiment()->saveToFile(path.toStdString());

    if (success){
        _save_path = path.toStdString();
        _saved = true;
    }

    return success;
}
