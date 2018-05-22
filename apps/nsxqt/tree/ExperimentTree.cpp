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
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/UnitCell.h>

#include "AbsorptionDialog.h"
#include "DataItem.h"
#include "DetectorItem.h"
#include "DetectorScene.h"
#include "DialogAutoIndexing.h"
#include "DialogExperiment.h"
#include "DialogRawData.h"
#include "DialogSpaceGroup.h"
#include "DialogTransformationMatrix.h"
#include "ExperimentItem.h"
#include "ExperimentTree.h"
#include "FriedelDialog.h"
#include "GLSphere.h"
#include "GLWidget.h"
#include "InstrumentItem.h"
#include "LibraryItem.h"
#include "MCAbsorptionDialog.h"
#include "NumorItem.h"
#include "PeakListItem.h"
#include "PeakTableView.h"
#include "ProgressView.h"
#include "QCustomPlot.h"
#include "SampleItem.h"
#include "SessionModel.h"
#include "SourceItem.h"
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

void ExperimentTree::onCustomMenuRequested(const QPoint& point)
{
    QModelIndex index = indexAt(point);
    QMenu* menu = new QMenu(this);
    SessionModel* session = dynamic_cast<SessionModel*>(model());
    auto triggered = &QAction::triggered;

    if (index == rootIndex()) {
        QAction* newexp = menu->addAction("Add new experiment");
        menu->popup(viewport()->mapToGlobal(point));
        connect(newexp, triggered, [=]() {session->createNewExperiment();});
    } else {
        QStandardItem* item = session->itemFromIndex(index);
        
        if (auto ditem = dynamic_cast<DataItem*>(item)) {            
            QAction* import = menu->addAction("Import data");
            QAction* rawImport = menu->addAction("Import raw data...");
            QAction* findpeaks = menu->addAction("Peak finder");
            connect(import, &QAction::triggered, [=](){ditem->importData();});
            // todo: fix this!!
            //connect(rawImport, &QAction::triggered, [=](){ditem->importRawData();});
            connect(findpeaks, &QAction::triggered, [=](){ditem->findPeaks();});
            menu->popup(viewport()->mapToGlobal(point));
        }
        else if (auto pitem = dynamic_cast<PeaksItem*>(item)) {
            QAction* abs = menu->addAction("Correct for Absorption");
            QAction* scene3d = menu->addAction("Show 3D view");
            QAction* library = menu->addAction("Build shape library");
            QAction* filter = menu->addAction("Filter peaks");
            connect(abs, triggered, [=]{pitem->absorptionCorrection();});
            connect(scene3d, triggered, [=]{pitem->showPeaksOpenGL();});
            connect(library, triggered, [=]{pitem->buildShapeLibrary();});
            connect(filter, triggered, [=](){pitem->filterPeaks();});
        }
        else if (SampleItem* sitem = dynamic_cast<SampleItem*>(item)) {
            QMenu* menu = new QMenu(this);
            QAction* addUnitCell = menu->addAction("Add unit cell");
            menu->popup(viewport()->mapToGlobal(point));
            connect(addUnitCell, &QAction::triggered, [=](){sitem->addUnitCell();});
        }
        else if (UnitCellItem* ucitem = dynamic_cast<UnitCellItem*>(item)) {
            QMenu* menu = new QMenu(this);
            QAction* info = menu->addAction("Info");
            menu->addSeparator();
            QAction* setTolerance = menu->addAction("Set HKL tolerance");
            menu->addSeparator();
            QAction* cellParameters=menu->addAction("Change unit cell parameters");
            QAction* transformationMatrix=menu->addAction("Transformation matrix");
            menu->popup(viewport()->mapToGlobal(point));

            connect(info, &QAction::triggered,[=]{ucitem->info();});
            connect(cellParameters, &QAction::triggered, [=]{ucitem->openChangeUnitCellDialog();});
            connect(transformationMatrix, &QAction::triggered, [=]{ucitem->openTransformationMatrixDialog();});

            connect(setTolerance, SIGNAL(triggered()),this, SLOT(setIndexingTolerance()));

        }
        else if (NumorItem* nitem = dynamic_cast<NumorItem*>(item)) {
            QAction* export_hdf = menu->addAction("Export to HDF5...");            

            auto export_fn = [=] {
                QString filename = QFileDialog::getSaveFileName(this, "Save File", "", "HDF5 (*.hdf *.hdf5)", nullptr, QFileDialog::Option::DontUseNativeDialog);
                nitem->exportHDF5(filename.toStdString());
            };
            connect(export_hdf, &QAction::triggered, this, export_fn);
        } else if (LibraryItem* lib_item = dynamic_cast<LibraryItem*>(item)) {
            QAction* predict = menu->addAction("Predict peaks");
            connect(predict, triggered, [=](){lib_item->incorporateCalculatedPeaks();});
        }
    }
    menu->popup(viewport()->mapToGlobal(point));
}

void ExperimentTree::onDoubleClick(const QModelIndex& index)
{
    // Get the current item and check that is actually a Numor item. Otherwise, return.
    QStandardItem* item=dynamic_cast<SessionModel*>(model())->itemFromIndex(index);
    if (auto ptr=dynamic_cast<DataItem*>(item)) {
        if (ptr->model()->rowCount(ptr->index())==0) {
            ptr->importData();
        } else {
            for (auto i = 0; i < ptr->model()->rowCount(ptr->index());++i) {
                auto ci = ptr->child(i);                
                Qt::CheckState new_state = ci->checkState() == Qt::Unchecked ? Qt::Checked : Qt::Unchecked;
                ci->setCheckState(new_state);
            }
        }
    } else if (auto ptr=dynamic_cast<SampleItem*>(item)) {
        ptr->addUnitCell();
    } else if (auto ptr=dynamic_cast<NumorItem*>(item)) {
        emit plotData(ptr->getData());
    }
}

void ExperimentTree::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        QList<QModelIndex> selIndexes = selectedIndexes();

        QListIterator<QModelIndex> it(selIndexes);
        it.toBack();
        while (it.hasPrevious()) {
            QStandardItem* item = dynamic_cast<SessionModel*>(model())->itemFromIndex(it.previous());
            if (!item->parent()) {
                model()->removeRow(item->row());
                emit resetScene();
            } else {
                model()->removeRow(item->row(),item->parent()->index());
                emit resetScene();
            }
        }
    }
}


void ExperimentTree::onSingleClick(const QModelIndex &index)
{
    // Inspect this item if it is inspectable
    InspectableTreeItem* item = dynamic_cast<InspectableTreeItem*>(dynamic_cast<SessionModel*>(model())->itemFromIndex(index));
    if (item) {
        emit inspectWidget(item->inspectItem());
    } else {
        QWidget* widget = new QWidget();
        emit inspectWidget(widget);
    }
}
