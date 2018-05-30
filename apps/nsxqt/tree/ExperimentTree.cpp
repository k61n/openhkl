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

#include <nsxlib/Diffractometer.h>
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
#include "DialogIsotopesDatabase.h"
#include "DialogRawData.h"
#include "DialogSpaceGroup.h"
#include "DialogTransformationMatrix.h"
#include "ExperimentItem.h"
#include "ExperimentTree.h"
#include "GLSphere.h"
#include "GLWidget.h"
#include "InstrumentItem.h"
#include "LibraryItem.h"
#include "MCAbsorptionDialog.h"
#include "NumorItem.h"
#include "PeaksItem.h"
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
        connect(newexp, triggered, [=]() {session->createNewExperiment();});
    } else {
        QStandardItem* item = session->itemFromIndex(index);
        
        if (auto exp_item = dynamic_cast<ExperimentItem*>(item)) {
            QAction* log = menu->addAction("Write detailed log files");
            connect(log, triggered, [=](){exp_item->writeLogFiles();});
        }
        else if (auto ditem = dynamic_cast<DataItem*>(item)) {            
            QAction* convert_to_hdf5 = menu->addAction("Convert to HDF5");
            QAction* import = menu->addAction("Import data");
            QAction* findpeaks = menu->addAction("Find peaks in data");
            connect(convert_to_hdf5, &QAction::triggered, [=](){ditem->convertToHDF5();});
            connect(import, &QAction::triggered, [=](){ditem->importData();});
            connect(findpeaks, &QAction::triggered, [=](){ditem->findPeaks();});
        }
        else if (auto pitem = dynamic_cast<PeaksItem*>(item)) {
            QAction* filter = menu->addAction("Filter peaks");
            QAction* autoidx = menu->addAction("FFT-autoindex peaks");
            QAction* assign = menu->addAction("Autoindex existing lattice");
            QAction* refine = menu->addAction("Refine lattice and instrument parameters");
            QAction* library = menu->addAction("Build shape library");
            QAction* integrate = menu->addAction("Integrate peaks");
            QAction* abs = menu->addAction("Correct for Absorption");
            QAction* scene3d = menu->addAction("Show 3D view");                      
            connect(abs, triggered, [=]{pitem->absorptionCorrection();});
            connect(scene3d, triggered, [=]{pitem->showPeaksOpenGL();});
            connect(library, triggered, [=]{pitem->buildShapeLibrary();});
            connect(filter, triggered, [=](){pitem->filterPeaks();});
            connect(integrate, triggered, [=](){pitem->integratePeaks();});
            connect(autoidx, triggered, [=](){pitem->autoindex();});
            connect(refine, triggered, [=](){pitem->refine();});
            connect(assign, triggered, [=](){pitem->autoAssignUnitCell();});
        }
        else if (SampleItem* sitem = dynamic_cast<SampleItem*>(item)) {
            QAction* openIsotopesDatabase = menu->addAction("Open isotopes database");
            connect(openIsotopesDatabase, &QAction::triggered, [=](){sitem->openIsotopesDatabase();});
        }
        else if (UnitCellItem* ucitem = dynamic_cast<UnitCellItem*>(item)) {
            QAction* info = menu->addAction("Info");
            menu->addSeparator();
            QAction* setTolerance = menu->addAction("Set HKL tolerance");
            menu->addSeparator();
            QAction* cellParameters=menu->addAction("Change unit cell parameters");
            QAction* transformationMatrix=menu->addAction("Transformation matrix");
            QAction* group = menu->addAction("Choose space group");

            connect(info, &QAction::triggered,[=]{ucitem->info();});
            connect(cellParameters, &QAction::triggered, [=]{ucitem->openChangeUnitCellDialog();});
            connect(transformationMatrix, &QAction::triggered, [=]{ucitem->openTransformationMatrixDialog();});
            connect(setTolerance, SIGNAL(triggered()),this, SLOT(setIndexingTolerance()));
            connect(group, triggered, [=](){ucitem->determineSpaceGroup();});

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
