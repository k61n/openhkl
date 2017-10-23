#include <array>
#include <memory>
#include <map>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include <hdf5.h>
#include <H5Exception.h>

#include <QAbstractItemView>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeyEvent>
#include <QList>
#include <QListIterator>
#include <QMenu>
#include <QModelIndexList>
#include <QStandardItem>
#include <QString>
#include <QVector>

#include <nsxlib/Experiment.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/UnitCell.h>

#include "AbsorptionDialog.h"
#include "DataItem.h"
#include "DetectorItem.h"
#include "DetectorScene.h"
#include "DialogAutoIndexing.h"
#include "DialogConvolve.h"
#include "DialogExperiment.h"
#include "DialogRawData.h"
#include "DialogTransformationMatrix.h"
#include "ExperimentItem.h"
#include "ExperimentTree.h"
#include "FriedelDialog.h"
#include "GLSphere.h"
#include "GLWidget.h"
#include "InstrumentItem.h"
#include "MCAbsorptionDialog.h"
#include "NumorItem.h"
#include "PeakListItem.h"
#include "PeakTableView.h"
#include "ProgressView.h"
#include "QCustomPlot.h"
#include "SampleItem.h"
#include "SessionModel.h"
#include "SourceItem.h"
#include "SpaceGroupDialog.h"
#include "TreeItem.h"
#include "UnitCellItem.h"

#include "ui_MainWindow.h"
#include "ui_ScaleDialog.h"

ExperimentTree::ExperimentTree(QWidget *parent):
    QTreeView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    expandAll();
    setSelectionMode(QAbstractItemView::ContiguousSelection);
    update();

    setExpandsOnDoubleClick(false);

    connect(this,SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onCustomMenuRequested(const QPoint&)));
    connect(this,SIGNAL(doubleClicked(const QModelIndex&)),this,SLOT(onDoubleClick(const QModelIndex&)));
    connect(this,SIGNAL(clicked(QModelIndex)),this,SLOT(onSingleClick(QModelIndex)));
}

ExperimentTree::~ExperimentTree()
{
}

void ExperimentTree::setSession(std::shared_ptr<SessionModel> session)
{
    _session = session;
    setModel(_session.get());
}

void ExperimentTree::setHKLTolerance()
{
    QStandardItem* item=_session->itemFromIndex(currentIndex());
    auto ucitem=dynamic_cast<UnitCellItem*>(item);
    if (!ucitem)
        return;

    bool ok;
    double tolerance = QInputDialog::getDouble(this,tr("HKL integer tolerance"),tr("value:"),ucitem->getUnitCell()->getHKLTolerance(),0.0,1.0,2,&ok);
    if (!ok)
        return;

    ucitem->getUnitCell()->setHKLTolerance(tolerance);

    onSingleClick(currentIndex());
}

void ExperimentTree::createNewExperiment()
{
    std::unique_ptr<DialogExperiment> dlg;

    // DialogExperiment could throw an exception if it fails to read the resource files
    try {
        dlg = std::unique_ptr<DialogExperiment>(new DialogExperiment());

        // The user pressed cancel, return
        if (!dlg->exec())
            return;

        // If no experiment name is provided, pop up a warning
        if (dlg->getExperimentName().isEmpty()) {
            nsx::error() << "Empty experiment name";
            return;
        }
    }
    catch(std::exception& e) {
        nsx::error() << e.what();
        return;
    }

    // Add the experiment
    try {
        _session->addExperiment(dlg->getExperimentName().toStdString(),dlg->getInstrumentName().toStdString());
    }
    catch(const std::runtime_error& e) {
        nsx::error() << e.what();
        return;
    }
}

void ExperimentTree::onCustomMenuRequested(const QPoint& point)
{
    QModelIndex index = indexAt(point);

    if (index == rootIndex()) {
        QMenu* menu = new QMenu(this);
        QAction* newexp = menu->addAction("Add new experiment");
        menu->popup(viewport()->mapToGlobal(point));
        connect(newexp, SIGNAL(triggered()), this, SLOT(createNewExperiment()));
    }
    else {
        QStandardItem* item = _session->itemFromIndex(index);
        if (dynamic_cast<DataItem*>(item))
        {
            QMenu* menu = new QMenu(this);
            QAction* import = menu->addAction("Import data");
            QAction* rawImport = menu->addAction("Import raw data...");
            QAction* findpeaks = menu->addAction("Peak finder");
            menu->popup(viewport()->mapToGlobal(point));
            connect(import, SIGNAL(triggered()), this, SLOT(importData()));
            connect(rawImport, SIGNAL(triggered()), this, SLOT(importRawData()));
            connect(findpeaks, &QAction::triggered, [=](){findPeaks(index);});
        }
        else if (dynamic_cast<PeakListItem*>(item))
        {
            QMenu* menu = new QMenu(this);
            QAction* abs = menu->addAction("Correct for Absorption");
            QAction* scene3d = menu->addAction("Show 3D view");
            menu->popup(viewport()->mapToGlobal(point));
            // Call the slot
            connect(abs, SIGNAL(triggered()), this, SLOT(absorptionCorrection()));
            connect(scene3d, SIGNAL(triggered()), this, SLOT(showPeaksOpenGL()));
        }
        else if (SampleItem* sitem=dynamic_cast<SampleItem*>(item))
        {
            QMenu* menu = new QMenu(this);
            QAction* addUnitCell = menu->addAction("Add unit cell");
            menu->popup(viewport()->mapToGlobal(point));

            auto addUnitCellLambda = [=] {sitem->addUnitCell();};
            connect(addUnitCell, &QAction::triggered, this, addUnitCellLambda);

        }
        else if (UnitCellItem* ucitem=dynamic_cast<UnitCellItem*>(item))
        {
            QMenu* menu = new QMenu(this);
            QAction* info = menu->addAction("Info");
            menu->addSeparator();
            QAction* setTolerance = menu->addAction("Set HKL tolerance");
            menu->addSeparator();
            QAction* cellParameters=menu->addAction("Change unit cell parameters");
            QAction* transformationMatrix=menu->addAction("Transformation matrix");
            menu->popup(viewport()->mapToGlobal(point));

            auto infoLambda = [=]{ucitem->info();};
            connect(info, &QAction::triggered, this, infoLambda);

            auto cellParametersLambda = [=]{ucitem->openChangeUnitCellDialog();};
            connect(cellParameters, &QAction::triggered, this, cellParametersLambda);

            auto transformationMatrixLambda = [=]{ucitem->openTransformationMatrixDialog();};
            connect(transformationMatrix, &QAction::triggered, this, transformationMatrixLambda);

            connect(setTolerance, SIGNAL(triggered()),this, SLOT(setHKLTolerance()));

        }
        else if (NumorItem* nitem = dynamic_cast<NumorItem*>(item))
            {
            QMenu* menu = new QMenu(this);
            QAction* export_hdf = menu->addAction("Export to HDF5...");
            menu->popup(viewport()->mapToGlobal(point));

            auto export_fn = [=] {
                QString filename = QFileDialog::getSaveFileName(this, "Save File", "", "HDF5 (*.hdf *.hdf5)", nullptr, QFileDialog::Option::DontUseNativeDialog);
                nitem->exportHDF5(filename.toStdString());
            };

            connect(export_hdf, &QAction::triggered, this, export_fn);
        }
    }
}

void ExperimentTree::absorptionCorrection()
{
    // Get the current item and check that is actually a Data item. Otherwise, return.
    QStandardItem* item=_session->itemFromIndex(currentIndex());
    auto pitem=dynamic_cast<PeakListItem*>(item);
    if (!pitem)
        return;
    MCAbsorptionDialog* dialog = new MCAbsorptionDialog(pitem->getExperiment(), this);
    dialog->open();
}

void ExperimentTree::importData()
{
    // Get the current item and check that is actually a Data item. Otherwise, return.
    DataItem* dataItem = dynamic_cast<DataItem*>(_session->itemFromIndex(currentIndex()));
    if (!dataItem)
        return;

    QStringList fileNames;
    fileNames = QFileDialog::getOpenFileNames(this,"select numors","","",nullptr,QFileDialog::Option::DontUseNativeDialog);

    // No files selected, do nothing
    if (fileNames.isEmpty())
        return;

    for (int i = 0; i < fileNames.size(); ++i) {
        dataItem->importData(fileNames[i].toStdString());
    }
}

void ExperimentTree::importRawData()
{
    // Get the current item and check that is actually a Data item. Otherwise, return.
    DataItem* dataItem = dynamic_cast<DataItem*>(_session->itemFromIndex(currentIndex()));

    if (!dataItem)
        return;

    auto exmt = dataItem->getExperiment();

    if (!exmt)
        return;

    QStringList files;
    files = QFileDialog::getOpenFileNames(this,"import raw data","","",nullptr,QFileDialog::Option::DontUseNativeDialog);


    files.sort();

    if (files.isEmpty())
        return;

    DialogRawData dialog(this);

    if (!dialog.exec())
        return;

    const double wavelength = dialog.wavelength();
    const double delta_phi = dialog.deltaPhi();
    const double delta_omega = dialog.deltaOmega();
    const double delta_chi = dialog.deltaChi();
    const bool swap_endian = dialog.swapEndian();
    const int bpp = dialog.bpp();
    const bool row_major = dialog.rowMajor();

    std::vector<std::string> filenames;

    for (auto&& file: files)
        filenames.push_back(file.toStdString());

    dataItem->importRawData(filenames, wavelength, delta_chi, delta_omega, delta_phi, row_major, swap_endian, bpp);
}



void ExperimentTree::findPeaks(const QModelIndex& index)
{
    _session->findPeaks(index);
}

void ExperimentTree::onDoubleClick(const QModelIndex& index)
{
    // Get the current item and check that is actually a Numor item. Otherwise, return.
    QStandardItem* item=_session->itemFromIndex(index);
    if (auto ptr=dynamic_cast<DataItem*>(item)) {
        if (ptr->model()->rowCount(ptr->index())==0)
            importData();
        else {
            for (auto i=0;i<ptr->model()->rowCount(ptr->index());++i) {
                if (ptr->child(i)->checkState() == Qt::Unchecked)
                    ptr->child(i)->setCheckState(Qt::Checked);
                else
                    ptr->child(i)->setCheckState(Qt::Unchecked);
            }
        }
    }
    else if (auto ptr=dynamic_cast<SampleItem*>(item))
        ptr->addUnitCell();
    else if (auto ptr=dynamic_cast<NumorItem*>(item)) {
        auto exp = ptr->getExperiment();
        emit plotData(exp->getData(item->text().toStdString()));
    }
}

void ExperimentTree::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        QList<QModelIndex> selIndexes = selectedIndexes();

        QListIterator<QModelIndex> it(selIndexes);
        it.toBack();
        while (it.hasPrevious()) {
            QStandardItem* item = _session->itemFromIndex(it.previous());
            if (!item->parent()) {
                _session->removeRow(item->row());
                emit resetScene();
            } else {
                _session->removeRow(item->row(),item->parent()->index());
                emit resetScene();
            }
        }
    }
}


void ExperimentTree::onSingleClick(const QModelIndex &index)
{
    // Inspect this item if it is inspectable
    InspectableTreeItem* item = dynamic_cast<InspectableTreeItem*>(_session->itemFromIndex(index));
    if (item) {
        emit inspectWidget(item->inspectItem());
    } else {
        QWidget* widget = new QWidget();
        emit inspectWidget(widget);
    }
}

void ExperimentTree::showPeaksOpenGL()
{
    GLWidget* glw = new GLWidget();
    auto& scene = glw->getScene();
    auto datav = _session->getSelectedNumors();

    for (auto idata : datav) {
       auto peaks=idata->getPeaks();
       for (auto peak: peaks) {
           GLSphere* sphere=new GLSphere("");
           Eigen::Vector3d pos=peak->getQ();
           sphere->setPos(pos[0]*100,pos[1]*100,pos[2]*100);
           sphere->setColor(0,1,0);
           scene.addActor(sphere);
       }
    }
    glw->show();
}

void ExperimentTree::findSpaceGroup()
{
    SpaceGroupDialog* dialog = new SpaceGroupDialog(_session->getSelectedNumors(), this);
    dialog->exec();
    // update the space group elsewhere
}

void ExperimentTree::computeRFactors()
{
    _session->computeRFactors();
}

void ExperimentTree::findFriedelPairs()
{
    nsx::error() << "findFriedelParis() is not yet implemented!";
    return;

}

void ExperimentTree::peakFitDialog()
{
    _session->peakFitDialog();
}

void ExperimentTree::incorporateCalculatedPeaks()
{
    _session->incorporateCalculatedPeaks();
}
