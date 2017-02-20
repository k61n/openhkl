#include <QIcon>
#include <QFileInfo>
#include <QDebug>
#include <QStandardItem>
#include <QJsonArray>

#include <nsxlib/data/RawData.h>
#include <nsxlib/instrument/Experiment.h>
#include "models/DataItem.h"
#include <nsxlib/data/DataReaderFactory.h>
#include "NumorItem.h"

using SX::Data::IData;
using SX::Data::DataReaderFactory;

DataItem::DataItem(std::shared_ptr<Experiment> experiment) : TreeItem(experiment)
{
    setText("Data");
    QIcon icon(":/resources/dataIcon.png");
    setIcon(icon);
    setEditable(false);
    setSelectable(false);
}

NumorItem *DataItem::importData(std::shared_ptr<IData> data)
{
    // Get the basename of the current numor
    QString filename(data->getFilename().c_str());
    QFileInfo fileinfo(filename);
    std::string basename = fileinfo.fileName().toStdString();
    std::shared_ptr<SX::Instrument::Experiment> exp = getExperiment();

    // If the experience already stores the current numor, skip it
    if (exp->hasData(basename))
        return nullptr;

    try {
        exp->addData(data);
    }
    catch(std::exception& e) {
        qWarning() << "Error reading numor: " + filename + " " + QString(e.what());
        return nullptr;
    }
    catch(...)  {
        qWarning() << "Error reading numor: " + filename + " reason not known:";
        return nullptr;
    }

    NumorItem* item = new NumorItem(exp, data);
    item->setText(QString::fromStdString(basename));
    item->setCheckable(true);
    appendRow(item);

    return item;
}


NumorItem* DataItem::importData(const std::string &filename_str)
{
    // Get the basename of the current numor
    QString filename(filename_str.c_str());
    QFileInfo fileinfo(filename);
    std::string basename = fileinfo.fileName().toStdString();
    std::shared_ptr<SX::Instrument::Experiment> exp = getExperiment();

    // If the experience already stores the current numor, skip it
    if (exp->hasData(basename))
        return nullptr;

    std::shared_ptr<IData> data_ptr;

    try {
        std::string extension = fileinfo.completeSuffix().toStdString();

        IData* raw_ptr = DataReaderFactory::Instance()->create(
                    extension, filename_str, exp->getDiffractometer()
                    );

        data_ptr = std::shared_ptr<IData>(raw_ptr);
    }
    catch(std::exception& e) {
        qWarning() << "Error reading numor: " + filename + " " + QString(e.what());
        return nullptr;
    }
    catch(...)  {
        qWarning() << "Error reading numor: " + filename + " reason not known:";
        return nullptr;
    }

    return importData(data_ptr);
}

NumorItem *DataItem::importRawData(const std::vector<std::string> &filenames,
                                   double wavelength, double delta_chi, double delta_omega, double delta_phi,
                                   bool rowMajor, bool swapEndian, int bpp)
{
    // Get the basename of the current numor
    QString filename(filenames[0].c_str());
    QFileInfo fileinfo(filename);
    std::string basename = fileinfo.fileName().toStdString();
    std::shared_ptr<SX::Instrument::Experiment> exp = getExperiment();

    // If the experience already stores the current numor, skip it
    if (exp->hasData(basename))
        return nullptr;

    std::shared_ptr<IData> data;

    try {
        auto diff = exp->getDiffractometer();
        auto data_ptr = new SX::Data::RawData(filenames, diff,
                                              wavelength, delta_chi, delta_omega, delta_phi,
                                              rowMajor, swapEndian, bpp);
        data = std::shared_ptr<SX::Data::IData>(data_ptr);
    }
    catch(std::exception& e) {
        qWarning() << "Error reading numor: " + filename + " " + QString(e.what());
        return nullptr;
    }
    catch(...)  {
        qWarning() << "Error reading numor: " + filename + " reason not known:";
        return nullptr;
    }

    return importData(data);
}

QJsonObject DataItem::toJson()
{
    QJsonObject obj;
    QJsonArray numors;

    for(int i = 0; i < rowCount(); ++i) {
        NumorItem* item = dynamic_cast<NumorItem*>(this->child(i));
        assert(item != nullptr);
        numors.push_back(item->toJson());
    }

    obj["numors"] = numors;

    return obj;
}

void DataItem::fromJson(const QJsonObject &obj)
{
    QJsonArray numors = obj["numors"].toArray();

    for (auto&& numor: numors) {
        QString filename = numor.toObject()["filename"].toString();
        NumorItem* item = importData(filename.toStdString());
        item->fromJson(numor.toObject());
    }
}
