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
#include <QtDebug>

#include "DataReaderFactory.h"
#include "Detector.h"
#include "DialogExperiment.h"
#include "Diffractometer.h"
#include "Sample.h"
#include "Source.h"

#include "PeakTableView.h"
#include "Absorption/AbsorptionDialog.h"
#include "Tree/DataItem.h"
#include "Tree/DetectorItem.h"
#include "Tree/ExperimentItem.h"
#include "Tree/ExperimentTree.h"
#include "Tree/TreeItem.h"
#include "Tree/InstrumentItem.h"
#include "Tree/NumorItem.h"
#include "Tree/PeakListItem.h"
#include "Tree/SampleItem.h"
#include "Tree/SourceItem.h"
#include "Absorption/DialogMCAbsorption.h"

ExperimentTree::ExperimentTree(QWidget *parent) : QTreeView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    _model=new QStandardItemModel();
    setModel(_model);
    expandAll();
    setSelectionMode(QAbstractItemView::ContiguousSelection);
    update();

    setExpandsOnDoubleClick(false);

    connect(this,SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onCustomMenuRequested(const QPoint&)));
    connect(this,SIGNAL(doubleClicked(const QModelIndex&)),this,SLOT(onDoubleClick(const QModelIndex&)));
    connect(this,SIGNAL(clicked(QModelIndex)),this,SLOT(onSingleClick(QModelIndex)));
}

void ExperimentTree::createNewExperiment()
{

    DialogExperiment* dlg = new DialogExperiment();

    // The user pressed cancel, return
    if (!dlg->exec())
        return;

    // If no experiment name is provided, pop up a warning
    if (dlg->getExperimentName().isEmpty())
    {
        qWarning() << "Empty experiment name";
        return;
    }

    // Add the experiment
    try
    {
        addExperiment(dlg->getExperimentName().toStdString(),dlg->getInstrumentName().toStdString());
    }
    catch(const std::runtime_error& e)
    {
        qWarning() << e.what();
        return;
    }
}

void ExperimentTree::addExperiment(const std::string& experimentName, const std::string& instrumentName)
{

    // Create an experiment
    SX::Instrument::Experiment* expPtr = new SX::Instrument::Experiment(experimentName,instrumentName);

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

std::vector<SX::Data::IData*> ExperimentTree::getSelectedNumors(ExperimentItem* item) const
{
    std::vector<SX::Data::IData*> numors;

    QList<QStandardItem*> dataItems = _model->findItems(QString("Data"),Qt::MatchCaseSensitive|Qt::MatchRecursive);

    for (const auto& it : dataItems)
    {
        for (auto i=0;i<_model->rowCount(it->index());++i)
        {
            if (it->child(i)->checkState() == Qt::Checked)
            {
                if (auto ptr = dynamic_cast<NumorItem*>(it->child(i)))
                {
                    if (it->parent() == item)
                        numors.push_back(ptr->getExperiment()->getData(ptr->text().toStdString()));
                }
            }
        }
    }

    return numors;
}

std::vector<SX::Data::IData*> ExperimentTree::getSelectedNumors() const
{

    std::vector<SX::Data::IData*> numors;

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


    if (index == rootIndex())
    {
        QMenu* menu = new QMenu(this);
        QAction* newexp=menu->addAction("Add new experiment");
        menu->popup(viewport()->mapToGlobal(point));
        connect(newexp,SIGNAL(triggered()),this,SLOT(createNewExperiment()));

    }
    else
    {
        QStandardItem* item=_model->itemFromIndex(index);
        if (dynamic_cast<DataItem*>(item))
        {
            QMenu* menu = new QMenu(this);
            QAction* import=menu->addAction("Import");
            menu->popup(viewport()->mapToGlobal(point));
            connect(import,SIGNAL(triggered()),this,SLOT(importData()));
        }
        else if (dynamic_cast<PeakListItem*>(item))
        {
            QMenu* menu = new QMenu(this);
            QAction* abs=menu->addAction("Correct for Absorption");
            menu->popup(viewport()->mapToGlobal(point));
            // Call the slot
            connect(abs,SIGNAL(triggered()),this,SLOT(absorptionCorrection()));
        }
    }


}

void ExperimentTree::absorptionCorrection()
{
    // Get the current item and check that is actually a Data item. Otherwise, return.
    QStandardItem* item=_model->itemFromIndex(currentIndex());
    auto pitem=dynamic_cast<PeakListItem*>(item);
    if (!pitem)
        return;
    DialogMCAbsorption* dialog=new DialogMCAbsorption(pitem->getExperiment(),this);
    dialog->open();
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
        std::string basename=fileinfo.fileName().toStdString();
         SX::Instrument::Experiment* exp = expItem->getExperiment();

        // If the experience already stores the current numor, skip it
        if (exp->hasData(basename))
            continue;

        SX::Data::IData* d;
        try
        {
            std::string extension=fileinfo.completeSuffix().toStdString();
            d = DataReaderFactory::Instance()->create(extension,fileNames[i].toStdString(),exp->getDiffractometer());
            exp->addData(d);
        }
        catch(std::exception& e)
        {
           qWarning() << "Error reading numor: " + fileNames[i] + " " + QString(e.what());
           continue;
        }

        QStandardItem* item = new NumorItem(exp,d);
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
        if (ptr->model()->rowCount(ptr->index())==0)
            importData();
        else
        {
            for (auto i=0;i<ptr->model()->rowCount(ptr->index());++i)
            {
                if (ptr->child(i)->checkState() == Qt::Unchecked)
                    ptr->child(i)->setCheckState(Qt::Checked);
                else
                    ptr->child(i)->setCheckState(Qt::Unchecked);
            }
        }
    }
    else if (auto ptr=dynamic_cast<NumorItem*>(item))
    {
        SX::Instrument::Experiment* exp = ptr->getExperiment();
        SX::Data::IData* data=exp->getData(item->text().toStdString());
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
            if (!item->parent())
                 _model->removeRow(item->row());
            else
                _model->removeRow(item->row(),item->parent()->index());
        }
    }
}

ExperimentItem* ExperimentTree::getExperimentItem(SX::Instrument::Experiment* exp)
{

    QModelIndex rootIdx = rootIndex();

    for (auto i=0;i<_model->rowCount(rootIdx);++i)
    {
        auto idx = _model->index(i,0,rootIdx);
        auto ptr=dynamic_cast<ExperimentItem*>(_model->itemFromIndex(idx));
        if (ptr && ptr->getExperiment()==exp)
            return ptr;
    }

    return nullptr;

}

void ExperimentTree::onSingleClick(const QModelIndex &index)
{
        // Inspect this item if it is inspectable
        InspectableTreeItem* item=dynamic_cast<InspectableTreeItem*>(_model->itemFromIndex(index));
        if (item)
            emit inspectWidget(item->inspectItem());
        else
        {
            QWidget* widget=new QWidget();
            emit inspectWidget(widget);
        }
}
