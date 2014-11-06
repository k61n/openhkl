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
#include <QModelIndexList>
#include <QStandardItem>
#include <QString>

#include "Detector.h"
#include "Diffractometer.h"
#include "InstrumentItem.h"
#include "DataItem.h"
#include "Detector.h"
#include "DetectorItem.h"
#include "TreeItem.h"
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

    Experiment* expPtr = new Experiment(experimentName,instrumentName);
    ExperimentItem* expt = new ExperimentItem(expPtr);

    // The instrument related sections
    InstrumentItem* instr = new InstrumentItem(expPtr);

    SampleItem* sample = new SampleItem(expPtr);
    SourceItem* source = new SourceItem(expPtr);
    DetectorItem* detector = new DetectorItem(expPtr);

    instr->appendRow(detector);
    instr->appendRow(sample);
    instr->appendRow(source);

    // The data related section
    DataItem* data = new DataItem(expPtr);

    expt->appendRow(instr);
    expt->appendRow(data);
    _model->appendRow(expt);
    update();

}

std::vector<std::string> ExperimentTree::getSelectedNumors() const
{
    QModelIndexList selIndexes = selectedIndexes();

    std::vector<std::string> numors;

    for (auto idx : selIndexes)
    {
        QStandardItem* item = _model->itemFromIndex(idx);
        if (auto ptr=dynamic_cast<NumorItem*>(item))
            numors.push_back(ptr->getExperiment()->getData(ptr->text().toStdString())->getFilename());
    }
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

    QStringList fileNames;
    fileNames= QFileDialog::getOpenFileNames(this,"select numors","","",nullptr,QFileDialog::Option::DontUseNativeDialog);

    // No files selected, do nothing
    if (fileNames.isEmpty())
        return;

    QModelIndex parentIndex = _model->parent(currentIndex());
    auto expItem=dynamic_cast<ExperimentItem*>(_model->itemFromIndex(parentIndex));

    for (int i=0;i<fileNames.size();++i)
    {
        // Get the basename of the current numor
        QFileInfo fileinfo(fileNames[i]);
        std::string basename=fileinfo.baseName().toStdString();

        Experiment* exp = expItem->getExperiment();

        // If the experience already stores the current numor, skip it
        if (exp->hasData(basename))
            continue;

        try
        {
            ILLAsciiData* d = new ILLAsciiData(fileNames[i].toStdString(),exp->getDiffractometer(),false);
            exp->addData(d);
        }
        catch(std::exception& e)
        {
           qWarning() << "Error reading numor: " + fileNames[i] + " " + QString(e.what());
           continue;
        }

        QStandardItem* item = new NumorItem(exp);
        item->setText(QString::fromStdString(basename));
        dataItem->appendRow(item);
    }

}

void ExperimentTree::onDoubleClick(const QModelIndex& index)
{

    // Get the current item and check that is actually a Numor item. Otherwise, return.
    QStandardItem* item=_model->itemFromIndex(index);
    if (!dynamic_cast<NumorItem*>(item))
        return;

    QModelIndex parentIndex = _model->parent(currentIndex());
    auto expItem=dynamic_cast<ExperimentItem*>(_model->itemFromIndex(_model->parent(parentIndex)));
//    std::string expName=exptItem->text().toStdString();
    Experiment* exp = expItem->getExperiment();

    IData* data=exp->getData(item->text().toStdString());

    emit plotData(data);

}

void ExperimentTree::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        std::cout<<"I DELETE"<<std::endl;
    }
}
