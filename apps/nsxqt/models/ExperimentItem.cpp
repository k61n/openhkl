#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>

#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>

#include "DataItem.h"
#include "DetectorItem.h"
#include "ExperimentItem.h"
#include "InstrumentItem.h"
#include "PeakListItem.h"
#include "SampleItem.h"
#include "SessionModel.h"
#include "SourceItem.h"
#include "TreeItem.h"

ExperimentItem::ExperimentItem(std::shared_ptr<SessionModel> session, nsx::sptrExperiment experiment) : TreeItem(experiment), _session(session)
{
    setText(QString::fromStdString(_experiment->getName()));
    setForeground(QBrush(QColor("blue")));
    QIcon icon(":/resources/experimentIcon.png");
    setIcon(icon);
    setDragEnabled(true);
    setDropEnabled(true);
    setEditable(true);

    _instr = new InstrumentItem(experiment);

    // Add the instrument item to the experiment item
    appendRow(_instr);

    // Create a data item and add it to the experiment item
    _data = new DataItem(experiment);
    appendRow(_data);

    // Create a peaks item and add it to the experiment item
     _peaks = new PeakListItem(_session, experiment);
    appendRow(_peaks);
}

QJsonObject ExperimentItem::toJson()
{
    auto exp_ptr = getExperiment();
    QJsonObject experiment;

    experiment["name"] = QString(exp_ptr->getName().c_str());
    experiment["instrument"] = _instr->toJson();
    experiment["data"] = _data->toJson();
    experiment["peaks"] = _peaks->toJson();

    return experiment;
}

void ExperimentItem::fromJson(const QJsonObject &obj)
{
    _instr->fromJson(obj["instrument"].toObject());
    _data->fromJson(obj["data"].toObject());
    _peaks->fromJson(obj["peaks"].toObject());
}

InstrumentItem* ExperimentItem::getInstrumentItem()
{
    return _instr;
}
