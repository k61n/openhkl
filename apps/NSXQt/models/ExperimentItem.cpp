#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>

#include "Diffractometer.h"
#include "IData.h"

#include "models/SessionModel.h"
#include "models/ExperimentItem.h"

#include "InstrumentItem.h"
#include "DetectorItem.h"
#include "SampleItem.h"
#include "SourceItem.h"
#include "DataItem.h"
#include "PeakListItem.h"

#include <QtDebug>

using SX::Instrument::Experiment;

ExperimentItem::ExperimentItem(std::shared_ptr<Experiment> experiment) : TreeItem(experiment)
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
     _peaks = new PeakListItem(experiment);
    appendRow(_peaks);
}

QJsonObject ExperimentItem::toJson()
{
    std::shared_ptr<SX::Instrument::Experiment> exp_ptr = getExperiment();
    QJsonObject experiment;

    experiment["name"] = QString(exp_ptr->getName().c_str());
    experiment["instrument"] = _instr->toJson();
    //experiment["instrument"] = exp_ptr->getDiffractometer()->getName().c_str();
    experiment["data"] = _data->toJson();
    experiment["peaks"] = _peaks->toJson();

    return experiment;
}

void ExperimentItem::fromJson(const QJsonObject &obj)
{
    SessionModel* session = dynamic_cast<SessionModel*>(model());
    assert(session != nullptr);

    _instr->fromJson(obj["instrument"].toObject());
    _data->fromJson(obj["data"].toObject());
    _peaks->fromJson(obj["peaks"].toObject());
}

InstrumentItem* ExperimentItem::getInstrumentItem()
{
    return _instr;
}
