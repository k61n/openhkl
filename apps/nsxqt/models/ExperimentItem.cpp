#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>

#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Source.h>

#include "DataItem.h"
#include "DetectorItem.h"
#include "ExperimentItem.h"
#include "InstrumentItem.h"
#include "LibraryItem.h"
#include "PeaksItem.h"
#include "SampleItem.h"
#include "SessionModel.h"
#include "SourceItem.h"
#include "TreeItem.h"

ExperimentItem::ExperimentItem(nsx::sptrExperiment experiment): TreeItem(), _experiment(experiment)
{
    setText(QString::fromStdString(experiment->getName()));
    setForeground(QBrush(QColor("blue")));
    QIcon icon(":/resources/experimentIcon.png");
    setIcon(icon);
    setDragEnabled(true);
    setDropEnabled(true);
    setEditable(true);

    auto diff = experiment->getDiffractometer();
    _instr = new InstrumentItem(diff->getName().c_str(), diff->getSource()->getName().c_str());

    // Add the instrument item to the experiment item
    appendRow(_instr);

    // Create a data item and add it to the experiment item
    _data = new DataItem;
    appendRow(_data);

    // Create a peaks item and add it to the experiment item
    _peaks = new PeaksItem;
    appendRow(_peaks);

    // Create the reference peak library
    _library = new LibraryItem;
    appendRow(_library);
}

QJsonObject ExperimentItem::toJson()
{
    auto exp_ptr = experiment();
    QJsonObject experiment;

    experiment["name"] = QString(exp_ptr->getName().c_str());
    experiment["instrument"] = _instr->toJson();
    experiment["data"] = _data->toJson();

    return experiment;
}

void ExperimentItem::fromJson(const QJsonObject &obj)
{
    _instr->fromJson(obj["instrument"].toObject());
    _data->fromJson(obj["data"].toObject());
}

InstrumentItem* ExperimentItem::getInstrumentItem()
{
    return _instr;
}
