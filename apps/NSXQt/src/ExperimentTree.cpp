#include "ExperimentTree.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <QAbstractItemView>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>
#include <QKeyEvent>
#include <QMenu>
#include <QStandardItem>
#include <QString>

#include "Diffractometer.h"
#include "InstrumentItem.h"
#include "DataItem.h"
#include "Detector.h"
#include "DetectorItem.h"
#include "ExperimentItem.h"
#include "ILLAsciiData.h"
#include "NumorItem.h"
#include "Sample.h"
#include "SampleItem.h"
#include "Source.h"
#include "SourceItem.h"

ExperimentTree::ExperimentTree(QWidget *parent) : QTreeView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    _model=new QStandardItemModel();
    setModel(_model);
    setSelectionMode(QAbstractItemView::ContiguousSelection);
    update();

    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onCustomMenuRequested(const QPoint&)));
    connect(this, SIGNAL(doubleClicked(const QModelIndex&)),this,SLOT(onDoubleClick(const QModelIndex&)));
}

void ExperimentTree::addExperiment(const std::string& experimentName, const std::string& instrumentName)
{

    // Throw if an experiment with the same name is already stored in the tree
    auto it = _experiments.find(experimentName);
    if (it != _experiments.end())
        throw std::runtime_error("Duplicate name for "+experimentName+" experiment");

    // Insert the new experiment in the _experiments map
    std::pair<std::string,Experiment> exp(experimentName,instrumentName);
    auto it1=_experiments.insert(exp);

    // Update the experiments tree
    ExperimentItem* expt=new ExperimentItem(experimentName);

    // The instrument related sections
    InstrumentItem* instr = new InstrumentItem(instrumentName);
    std::shared_ptr<Diffractometer> diff = it1.first->second.getDiffractometer();

    SampleItem* sample = new SampleItem(diff->getSample()->getName());
    SourceItem* source = new SourceItem(diff->getSource()->getName());
    DetectorItem* detector = new DetectorItem(diff->getDetector()->getName());
    instr->appendRow(detector);
    instr->appendRow(sample);
    instr->appendRow(source);

    // The data related section
    DataItem* data = new DataItem("data");

    expt->appendRow(instr);
    expt->appendRow(data);
    _model->appendRow(expt);
    update();

}

void ExperimentTree::onCustomMenuRequested(const QPoint& point)
{

    QModelIndex index = indexAt(point);
    QStandardItem* item=_model->itemFromIndex(index);
    if (dynamic_cast<DataItem*>(item))
    {
        QMenu* menu = new QMenu(this);
        QAction* import=menu->addAction("Import");
        menu->popup(viewport()->mapToGlobal(point));
        connect(import,SIGNAL(triggered()),this,SLOT(importData()));
    }
}

void ExperimentTree::importData()
{

    // Get the current item and check that is actually a Data item. Otherwise, return.
    QStandardItem* dataItem=_model->itemFromIndex(currentIndex());
    if (!dynamic_cast<DataItem*>(dataItem))
        return;

    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFiles);
    QStringList fileNames;
    fileNames= dialog.getOpenFileNames(this,"select numors","");

    // No files selected, do nothing
    if (fileNames.isEmpty())
        return;

    QModelIndex parentIndex = _model->parent(currentIndex());
    QStandardItem* exptItem=_model->itemFromIndex(parentIndex);

    for (int i=0;i<fileNames.size();++i)
    {
        // Get the basename of the current numor
        QFileInfo fileinfo(fileNames[i]);
        std::string index=fileinfo.baseName().toStdString();

        // Get the name of the experience that will hold the data
        std::string expName=exptItem->text().toStdString();

        Experiment& expt = _experiments.at(expName);

        // If the experience already stores the current numor, skip it
        if (expt.hasData(index))
            continue;

        try
        {
            ILLAsciiData* d = new ILLAsciiData(fileNames[i].toStdString(),expt.getDiffractometer(),false);
            expt.addData(d);
        }catch(std::exception& e)
        {
           qWarning() << "Error reading numor: " + fileNames[i] + " " + QString(e.what());
           continue;
        }

        dataItem->appendRow(new NumorItem(index));
    }

}

void ExperimentTree::onDoubleClick(const QModelIndex& index)
{

    // Get the current item and check that is actually a Numor item. Otherwise, return.
    QStandardItem* item=_model->itemFromIndex(index);
    if (!dynamic_cast<NumorItem*>(item))
        return;

    QModelIndex parentIndex = _model->parent(currentIndex());
    QStandardItem* exptItem=_model->itemFromIndex(_model->parent(parentIndex));
    std::string expName=exptItem->text().toStdString();
    Experiment& expt = _experiments.at(expName);

    IData* data=expt.getData(item->text().toStdString());

    emit sig_plot_data(data);

}

//void ExperimentTree::treat(QModelIndex newitem,QModelIndex olditem)
//{
//    Q_UNUSED(olditem);
//    QStandardItemModel* m=dynamic_cast<QStandardItemModel*>(model());
//    QStandardItem* item=m->itemFromIndex(newitem);
//    if (dynamic_cast<InstrumentItem*>(item))
//            std::cout << "Instrument" << std::endl;
//}

void ExperimentTree::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        std::cout<<"I DELETE"<<std::endl;
    }
}
