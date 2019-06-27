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

#include "gui/models/Session.h"
#include "core/instrument/HardwareParameters.h"
#include <QDateTime>
#include <QStringList>

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
    if (experiment_->data().empty()) {
        qWarning("No data in selected experiment");
        return nullptr;
    }
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

void SessionExperiment::addPeaks(nsx::PeakList peaks, const QString& listname,
                                 const QString& uppername)
{
    if (uppername.length()==0) {
        QMap<QString, nsx::PeakList> innerList;
        innerList.insert("all Peaks", peaks);
        peakLists_.insert(listname, innerList);
        return;
    }
    QString upperlist = uppername;
    if (uppername.contains('/'))
        upperlist = uppername.split('/').at(0);

    peakLists_[upperlist].insert(listname, peaks);
    gSession->onPeaksChanged();
}

nsx::PeakList SessionExperiment::getPeaks(int upperindex, int lowerindex)
{
    if (peakLists_.empty()) {
        nsx::PeakList emptyList;
        return emptyList;
    }

    QString outername;
    QString innername;
    if (upperindex == -1)
        return getPeaks(selectedList_);
    else
        outername = getPeakListNames(0).at(upperindex);
    QMap<QString, nsx::PeakList> innermap = peakLists_.value(outername);
    QStringList innernames = innermap.keys();
    if (lowerindex == -1)
        innername = innernames.at(0);
    else
        innername = innernames.at(lowerindex);
    return innermap.value(innername);
}

nsx::PeakList SessionExperiment::getPeaks(const QString& peakListName)
{
    QString searchedName;
    QString filteredName;
    if (!peakListName.contains("/")) {
        filteredName = "all Peaks";
        searchedName = peakListName;
    } else {
        QStringList listnames = peakListName.split("/");
        filteredName = listnames.at(1);
        searchedName = listnames.at(0);
    }

    return peakLists_.value(searchedName).value(filteredName);
}

QStringList SessionExperiment::getPeakListNames(int depth)
{
    QStringList outernames = peakLists_.keys();
    if (depth == 0)
        return outernames;
    QStringList allnames;
    for (QString outername : outernames) {
        QStringList innernames = peakLists_.value(outername).keys();
        for (QString innername : innernames)
            allnames.append(outername + "/" + innername);
    }
    return allnames;
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
    peakLists_[outername].remove(innername);

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
