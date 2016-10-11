#include <QIcon>
#include <QFileInfo>
#include <QDebug>
#include <QStandardItem>
#include <QJsonArray>

#include "Experiment.h"
#include "Tree/DataItem.h"
#include "DataReaderFactory.h"
#include "NumorItem.h"

DataItem::DataItem(std::shared_ptr<Experiment> experiment) : TreeItem(experiment)
{
    setText("Data");

    QIcon icon(":/resources/dataIcon.png");
    setIcon(icon);

    setEditable(false);
    setSelectable(false);
}


void DataItem::importData(const std::string &filename_str)
{
    // Get the basename of the current numor
    QString filename(filename_str.c_str());
    QFileInfo fileinfo(filename);
    std::string basename=fileinfo.fileName().toStdString();
    std::shared_ptr<SX::Instrument::Experiment> exp = getExperiment();

    // If the experience already stores the current numor, skip it
    if (exp->hasData(basename))
        return;

    std::shared_ptr<IData> data_ptr;

    try {
        std::string extension = fileinfo.completeSuffix().toStdString();

        IData* raw_ptr = DataReaderFactory::Instance()->create(
                    extension, filename_str, exp->getDiffractometer()
                    );

        data_ptr = std::shared_ptr<IData>(raw_ptr);

        exp->addData(data_ptr);
    }
    catch(std::exception& e) {
        qWarning() << "Error reading numor: " + filename + " " + QString(e.what());
        return;
    }
    catch(...)  {
        qWarning() << "Error reading numor: " + filename + " reason not known:";
        return;
    }

    QStandardItem* item = new NumorItem(exp, data_ptr);
    item->setText(QString::fromStdString(basename));
    item->setCheckable(true);
    appendRow(item);
}

QJsonObject DataItem::toJson()
{
    QJsonObject obj;
    QJsonArray numors;

    for(int i = 0; i < rowCount(); ++i) {
        std::string name = this->child(i)->text().toStdString();
        std::map<std::string, std::shared_ptr<IData>>::const_iterator it = getExperiment()->getData().find(name);

        assert(it != getExperiment()->getData().cend());

        const std::shared_ptr<IData>& data = it->second;
        assert(data != nullptr);
        numors.push_back(data->getFilename().c_str());
    }

    obj["numors"] = numors;

    return obj;
}

void DataItem::fromJson(const QJsonObject &obj)
{
    QJsonArray numors = obj["numors"].toArray();

    for (auto&& numor: numors)
        importData(numor.toString().toStdString());
}
