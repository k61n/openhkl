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

#include "DataItem.h"
#include "DetectorItem.h"
#include "DetectorScene.h"
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
#include "MainWindow.h"
#include "MetaTypes.h"
#include "NumorItem.h"
#include "PeaksItem.h"
#include "PeakListItem.h"
#include "PeakTableView.h"
#include "ProgressView.h"
#include "QCustomPlot.h"
#include "SampleItem.h"
#include "SessionModel.h"
#include "SessionModelDelegate.h"
#include "SourceItem.h"
#include "TreeItem.h"
#include "UnitCellItem.h"
#include "UnitCellsItem.h"

#include "ui_MainWindow.h"

#include <QDebug>

ExperimentTree::ExperimentTree(QWidget *parent)
    : QTreeView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    expandAll();
    setSelectionMode(QAbstractItemView::ContiguousSelection);
    update();

    setExpandsOnDoubleClick(false);

    auto session_model_delegate = new SessionModelDelegate();

    setItemDelegate(session_model_delegate);

    connect(this,SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onCustomMenuRequested(const QPoint&)));
    connect(this,SIGNAL(doubleClicked(const QModelIndex&)),this,SLOT(onDoubleClick(const QModelIndex&)));
    connect(this,SIGNAL(clicked(QModelIndex)),this,SLOT(onSingleClick(QModelIndex)));
}

ExperimentTree::~ExperimentTree()
{
}

void ExperimentTree::setMainWindow(MainWindow *main_window)
{
    _main_window = main_window;
}

SessionModel* ExperimentTree::session()
{
    auto session_model = dynamic_cast<SessionModel*>(model());

    return session_model;
}

void ExperimentTree::onCustomMenuRequested(const QPoint& point)
{
    QModelIndex index = indexAt(point);
    QMenu* menu = new QMenu(this);
    SessionModel* session = dynamic_cast<SessionModel*>(model());
    auto triggered = &QAction::triggered;

    if (index == rootIndex()) {
        QAction* newexp = menu->addAction("Add new experiment");
        connect(newexp, triggered, [=]() {_main_window->onNewExperiment();});
    } else {
        QStandardItem* item = session->itemFromIndex(index);
        
        if (auto exp_item = dynamic_cast<ExperimentItem*>(item)) {
            QAction* log = menu->addAction("Write detailed log files");
            connect(log, triggered, [=](){exp_item->writeLogFiles();});
        }
        else if (auto ditem = dynamic_cast<DataItem*>(item)) {            
            QAction* load_data = menu->addAction("Load data");
            connect(load_data, &QAction::triggered, [=](){ditem->importData();});

            QAction* remove_selected_data = menu->addAction("Remove selected data");
            connect(remove_selected_data, &QAction::triggered, [=](){ditem->removeSelectedData();});

            QAction* convert_to_hdf5 = menu->addAction("Convert to HDF5");
            connect(convert_to_hdf5, &QAction::triggered, [=](){ditem->convertToHDF5();});

            QAction* import_raw = menu->addAction("Import raw data");
            connect(import_raw, &QAction::triggered, [=](){ditem->importRawData();});

            QAction* open_instrument_states_dialog = menu->addAction("Open instrument states dialog");
            connect(open_instrument_states_dialog, &QAction::triggered, [=](){ditem->openInstrumentStatesDialog();});

            QAction* find_peaks = menu->addAction("Find peaks in data");
            connect(find_peaks, &QAction::triggered, [=](){emit openPeakFindDialog(ditem);});
        }
        else if (auto pitem = dynamic_cast<PeaksItem*>(item)) {

            QAction* filter = menu->addAction("Filter peaks");
            connect(filter, triggered, [=](){pitem->openPeakFilterDialog();});

            QAction* remove_selected_data = menu->addAction("Remove selected peak collections");
            connect(remove_selected_data, &QAction::triggered, [=](){pitem->removeSelectedPeakCollections();});

            QMenu *indexing_menu = new QMenu("Indexing");
            QAction* autoindex = indexing_menu->addAction("FFT auto indexer");
            connect(autoindex, triggered, [=](){pitem->openAutoIndexingFrame();});

            QAction* user_defined = indexing_menu->addAction("User defined cell parameters indexer");
            connect(user_defined, triggered, [=](){pitem->openUserDefinedUnitCellIndexerFrame();});

            QAction* assign = indexing_menu->addAction("Assign unit cell");
            connect(assign, triggered, [=](){pitem->autoAssignUnitCell();});

            menu->addMenu(indexing_menu);

            QAction* refine = menu->addAction("Refine lattice and instrument parameters");
            connect(refine, triggered, [=](){pitem->refine();});

            QAction* library = menu->addAction("Build shape library");
            connect(library, triggered, [=]{pitem->buildShapeLibrary();});

            QAction* integrate = menu->addAction("Integrate peaks");
            connect(integrate, triggered, [=](){pitem->integratePeaks();});

            QAction* normalize = menu->addAction("Normalize to monitor");
            connect(normalize, triggered, [=](){pitem->normalizeToMonitor();});

            QAction* abs = menu->addAction("Correct for Absorption");
            connect(abs, triggered, [=]{pitem->absorptionCorrection();});

            QAction* scene3d = menu->addAction("Show 3D view");
            connect(scene3d, triggered, [=]{pitem->showPeaksOpenGL();});
        }
        else if (SampleItem* sitem = dynamic_cast<SampleItem*>(item)) {
            QAction* openIsotopesDatabase = menu->addAction("Open isotopes database");
            connect(openIsotopesDatabase, &QAction::triggered, [=](){sitem->openIsotopesDatabase();});
            QAction* openSampleGlobalOffsets = menu->addAction("Sample goniometer global offsets");
            connect(openSampleGlobalOffsets, &QAction::triggered, [=](){sitem->openSampleGlobalOffsetsFrame();});
        }
        else if (DetectorItem* detector_item = dynamic_cast<DetectorItem*>(item)) {
            QAction* openDetectorGlobalOffsets = menu->addAction("Detector goniometer global offsets");
            connect(openDetectorGlobalOffsets, &QAction::triggered, [=](){detector_item->openDetectorGlobalOffsetsFrame();});
        }
        else if (UnitCellItem* ucitem = dynamic_cast<UnitCellItem*>(item)) {
            QAction* info = menu->addAction("Info");
            menu->addSeparator();
            QAction* setTolerance = menu->addAction("Set HKL tolerance");
            menu->addSeparator();
            QAction* cellParameters=menu->addAction("Change unit cell parameters");
            QAction* transformationMatrix=menu->addAction("Transformation matrix");
            QAction* group = menu->addAction("Choose space group");

            connect(info, &QAction::triggered,[=](){ucitem->info();});
            connect(cellParameters, &QAction::triggered, [=](){ucitem->openChangeUnitCellDialog();});
            connect(transformationMatrix, &QAction::triggered, [=](){ucitem->openTransformationMatrixDialog();});
            connect(setTolerance, &QAction::triggered,[=](){ucitem->openIndexingToleranceDialog();});
            connect(group, triggered, [=](){ucitem->openSpaceGroupDialog();});
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
        } else if (UnitCellsItem* unit_cells_item = dynamic_cast<UnitCellsItem*>(item)) {
            QAction* remove_unused_unit_cell = menu->addAction("Remove unused unit cells");
            connect(remove_unused_unit_cell, triggered, [=](){unit_cells_item->removeUnusedUnitCells();});
        } else {
            delete menu;
            return;
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
        session()->selectData(ptr->data(Qt::UserRole).value<nsx::sptrDataSet>());
    }
}

void ExperimentTree::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        QList<QModelIndex> selIndexes = selectedIndexes();

        QListIterator<QModelIndex> it(selIndexes);
        it.toBack();
        while (it.hasPrevious()) {
            auto item = dynamic_cast<SessionModel*>(model())->itemFromIndex(it.previous());
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

