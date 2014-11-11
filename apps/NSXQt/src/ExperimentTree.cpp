#include "ExperimentTree.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include <QAbstractItemView>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeyEvent>
#include <QList>
#include <QListIterator>
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
#include "PeakListItem.h"
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

    // Create an experiment
    Experiment* expPtr = new Experiment(experimentName,instrumentName);

    // Create an instrument item
    InstrumentItem* instr = new InstrumentItem(expPtr);

    // Create a detector item and add it to the instrument item
    DetectorItem* detector = new DetectorItem(expPtr);
    instr->appendRow(detector);

    // Create a sample item and add it to the instrument item
    SampleItem* sample = new SampleItem(expPtr);
    instr->appendRow(sample);

    // Create a source item and add it to the instrument leaf
    SourceItem* source = new SourceItem(expPtr);
    instr->appendRow(source);

    // Create an experiment item
    ExperimentItem* expt = new ExperimentItem(expPtr);

    // Add the instrument item to the experiment item
    expt->appendRow(instr);

    // Create a data item and add it to the experiment item
    DataItem* data = new DataItem(expPtr);
    expt->appendRow(data);

    // Create a peaks item and add it to the experiment item
    PeakListItem* peaks = new PeakListItem(expPtr);
    expt->appendRow(peaks);

    // Add the experiment item to the root of the experiment tree
    _model->appendRow(expt);

    update();

}

std::vector<IData*> ExperimentTree::getSelectedNumors() const
{

    std::vector<IData*> numors;

    QList<QStandardItem*> dataItems = _model->findItems(QString("Data"),Qt::MatchCaseSensitive|Qt::MatchRecursive);

    for (const auto& it : dataItems)
    {
        for (auto i=0;i<_model->rowCount(it->index());++i)
        {
            if (it->child(i)->checkState() == Qt::Checked)
            {
                if (auto ptr = dynamic_cast<NumorItem*>(it->child(i)))
                    numors.push_back(ptr->getExperiment()->getData(ptr->text().toStdString()));
            }
        }
    }

    return numors;
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
        item->setCheckable(true);
        dataItem->appendRow(item);
    }

}

void ExperimentTree::onDoubleClick(const QModelIndex& index)
{
    // Get the current item and check that is actually a Numor item. Otherwise, return.
    QStandardItem* item=_model->itemFromIndex(index);

    if (auto ptr=dynamic_cast<DataItem*>(item))
    {
        for (auto i=0;i<ptr->model()->rowCount(ptr->index());++i)
        {
            if (ptr->child(i)->checkState() == Qt::Unchecked)
                ptr->child(i)->setCheckState(Qt::Checked);
            else
                ptr->child(i)->setCheckState(Qt::Unchecked);
        }
    }
    else if (auto ptr=dynamic_cast<NumorItem*>(item))
    {
        Experiment* exp = ptr->getExperiment();
        IData* data=exp->getData(item->text().toStdString());
        emit plotData(data);
    }
}

void ExperimentTree::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        QList<QModelIndex> selIndexes = selectedIndexes();

        QListIterator<QModelIndex> it(selIndexes);
        it.toBack();
        while (it.hasPrevious())
        {
            QStandardItem* item = _model->itemFromIndex(it.previous());

            if (auto ptr=dynamic_cast<NumorItem*>(item))
            {
                ptr->getExperiment()->removeData(ptr->text().toStdString());
                _model->removeRow(ptr->row(),ptr->parent()->index());
            }
            else if (auto ptr=dynamic_cast<ExperimentItem*>(item))
                _model->removeRow(ptr->row());
        }
    }
}
