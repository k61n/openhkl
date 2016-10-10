#include <QIcon>
#include <QFileInfo>
#include <QDebug>
#include <QStandardItem>

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
