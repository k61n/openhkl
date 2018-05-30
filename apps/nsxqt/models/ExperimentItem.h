#pragma once

#include <string>
#include <memory>

#include <QJsonObject>
#include <QStandardItemModel>

#include <nsxlib/Experiment.h>
#include <nsxlib/InstrumentTypes.h>

#include "TreeItem.h"

class DataItem;
class LibraryItem;
class InstrumentItem;
class PeaksItem;

class ExperimentItem: public TreeItem
{
public:
    explicit ExperimentItem(nsx::sptrExperiment experiment);
    virtual ~ExperimentItem() = default;
    QJsonObject toJson();
    void fromJson(const QJsonObject& obj);

    nsx::sptrExperiment experiment() { return _experiment; }

    InstrumentItem* instrumentItem();

    PeaksItem* peaksItem();

    DataItem* dataItem();

    LibraryItem* libraryItem();

    void writeLogFiles();

private:
    nsx::sptrExperiment _experiment;
    InstrumentItem* _instr;
    DataItem* _data;
    PeaksItem* _peaks;
    LibraryItem* _library;
};
