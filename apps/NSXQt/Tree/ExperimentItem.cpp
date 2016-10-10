#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>

#include "Diffractometer.h"
#include "IData.h"

#include "models/SessionModel.h"
#include "Tree/ExperimentItem.h"

#include "InstrumentItem.h"
#include "DetectorItem.h"
#include "SampleItem.h"
#include "SourceItem.h"
#include "DataItem.h"
#include "PeakListItem.h"

//using SX::Instrument::Experiment;

ExperimentItem::ExperimentItem(std::shared_ptr<Experiment> experiment) : TreeItem(experiment)
{
    setText(QString::fromStdString(_experiment->getName()));
    setForeground(QBrush(QColor("blue")));
    QIcon icon(":/resources/experimentIcon.png");
    setIcon(icon);
    setDragEnabled(true);
    setDropEnabled(true);
    setEditable(true);

    InstrumentItem* instr = new InstrumentItem(experiment);

    // Create a detector item and add it to the instrument item
    DetectorItem* detector = new DetectorItem(experiment);
    instr->appendRow(detector);

    // Create a sample item and add it to the instrument item
    SampleItem* sample = new SampleItem(experiment);
    instr->appendRow(sample);

    // Create a source item and add it to the instrument leaf
    SourceItem* source = new SourceItem(experiment);
    instr->appendRow(source);

    // Add the instrument item to the experiment item
    appendRow(instr);

    // Create a data item and add it to the experiment item
    DataItem* data = new DataItem(experiment);
    appendRow(data);

    // Create a peaks item and add it to the experiment item
    PeakListItem* peaks = new PeakListItem(experiment);
    appendRow(peaks);
}

void ExperimentItem::setData(const QVariant &value, int role)
{
    QStandardItem::setData(value,role);
    _experiment->setName(text().toStdString());
}

ExperimentItem::~ExperimentItem()
{
    // no longer needed since _experiment is a smart pointer
    //if (_experiment)
    //    delete _experiment;
}

QJsonObject ExperimentItem::toJson()
{
    std::shared_ptr<SX::Instrument::Experiment> exp_ptr = getExperiment();

    QJsonObject experiment;
    QJsonArray datasets;

    for(auto&& name : exp_ptr->getDataNames())
        datasets.append(name.c_str());

    experiment["name"] = exp_ptr->getName().c_str();
    experiment["instrument"] = exp_ptr->getDiffractometer()->getName().c_str();
    experiment["data"] = datasets;
}

void ExperimentItem::fromJson(QJsonObject &obj)
{
    SessionModel* session = dynamic_cast<SessionModel*>(model());
    assert(session != nullptr);

    std::string name = obj["name"].toString().toStdString();
    std::string instrument = obj["instrument"].toString().toStdString();
    QJsonArray datasets = obj["data"].toArray();

    std::shared_ptr<SX::Instrument::Experiment> exp_ptr = session->addExperiment(name, instrument);

    for(QJsonValue&& val: datasets) {
        //exp_ptr->addData()
        std::string filename = val.toString().toStdString();



        if (exp_ptr->hasData(filename))
            continue;

        //std::shared_ptr<SX::Data::IData> data(new SX::Data::IData());
    }
}


//QJsonValue ExperimentItem::toJson()
//{
//    QJsonObject experiment;

//    experiment["name"] = getExperiment()->getName().c_str();

//    experiment["instrument"] = 0;

//    QJsonArray numors;

//    for(auto&& name: getExperiment()->getDataNames())
//        numors.push_back(name.c_str());

//    experiment["data"] = numors;

//    // experiment["peaks"] = 0;

//    return experiment;
//}
