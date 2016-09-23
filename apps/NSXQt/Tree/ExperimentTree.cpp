#include "ui_MainWindow.h"
#include "DialogConvolve.h"
#include "ProgressHandler.h"

#include <memory>
#include <stdexcept>
#include <utility>
#include <map>
#include <array>
#include <tuple>
#include <vector>

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

//#include "BlobFinder.h"
#include "DataReaderFactory.h"
#include "Detector.h"
#include "DialogExperiment.h"
#include "Diffractometer.h"
#include "Sample.h"
#include "Source.h"

#include "PeakTableView.h"
#include "AbsorptionDialog.h"
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
#include "MCAbsorptionDialog.h"
#include "OpenGL/GLWidget.h"
#include "OpenGL/GLSphere.h"
#include "Logger.h"
#include "ReciprocalSpaceViewer.h"
#include "DetectorScene.h"

#include "SpaceGroupSymbols.h"
#include "SpaceGroup.h"

#include "SpaceGroupDialog.h"

#include <QVector>
#include "Externals/qcustomplot.h"
#include "ui_ScaleDialog.h"

#include "ScaleDialog.h"
#include "FriedelDialog.h"

#include "RFactor.h"

using std::vector;
using SX::Data::IData;
using std::shared_ptr;
using SX::Utils::ProgressHandler;

ExperimentTree::ExperimentTree(QWidget *parent) : QTreeView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    _model=new QStandardItemModel(this);
    setModel(_model);
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
    // _model should be deleted automatically during destructor by QT
    //delete _model;
}

void ExperimentTree::createNewExperiment()
{
    DialogExperiment* dlg;

    // DialogExperiment could throw an exception if it fails to read the resource files
    try {
        dlg = new DialogExperiment();

        // The user pressed cancel, return
        if (!dlg->exec())
            return;

        // If no experiment name is provided, pop up a warning
        if (dlg->getExperimentName().isEmpty()) {
            qWarning() << "Empty experiment name";
            return;
        }
    }
    catch(std::exception& e) {
        qDebug() << "Runtime error: " << e.what();
        return;
    }

    // Add the experiment
    try {
        addExperiment(dlg->getExperimentName().toStdString(),dlg->getInstrumentName().toStdString());
    }
    catch(const std::runtime_error& e) {
        qWarning() << e.what();
        return;
    }
}

void ExperimentTree::addExperiment(const std::string& experimentName, const std::string& instrumentName)
{

    // Create an experiment
    std::shared_ptr<SX::Instrument::Experiment> expPtr(new SX::Instrument::Experiment(experimentName,instrumentName));

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

vector<shared_ptr<IData>> ExperimentTree::getSelectedNumors(ExperimentItem* item) const
{
    vector<shared_ptr<IData>> numors;

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

vector<shared_ptr<IData>> ExperimentTree::getSelectedNumors() const
{
    vector<shared_ptr<IData>> numors;

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
            QAction* findpeaks=menu->addAction("Peak finder");
            QAction* rviewer=menu->addAction("Reciprocal space viewer");
            menu->popup(viewport()->mapToGlobal(point));
            connect(import,SIGNAL(triggered()),this,SLOT(importData()));
            connect(findpeaks,&QAction::triggered,[=](){findPeaks(index);});
            connect(rviewer,&QAction::triggered,[=](){viewReciprocalSpace(index);});
        }
        else if (dynamic_cast<PeakListItem*>(item))
        {
            QMenu* menu = new QMenu(this);
            QAction* abs=menu->addAction("Correct for Absorption"); // Absorption menu
            QAction* scene3d=menu->addAction("Show 3D view"); // Peak in 3D OpenGL window
            menu->popup(viewport()->mapToGlobal(point));
            // Call the slot
            connect(abs,SIGNAL(triggered()),this,SLOT(absorptionCorrection()));
            connect(scene3d,SIGNAL(triggered()),this,SLOT(showPeaksOpenGL()));
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
    MCAbsorptionDialog* dialog = new MCAbsorptionDialog(pitem->getExperiment(), this);
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
         std::shared_ptr<SX::Instrument::Experiment> exp = expItem->getExperiment();

        // If the experience already stores the current numor, skip it
        if (exp->hasData(basename))
            continue;

        std::shared_ptr<IData> data_ptr;

        try
        {
            std::string extension=fileinfo.completeSuffix().toStdString();

            IData* raw_ptr = DataReaderFactory::Instance()->create(
                        extension,fileNames[i].toStdString(),exp->getDiffractometer()
                        );

            data_ptr = std::shared_ptr<IData>(raw_ptr);

            exp->addData(data_ptr);
        }
        catch(std::exception& e)
        {
           qWarning() << "Error reading numor: " + fileNames[i] + " " + QString(e.what());
           continue;
        }

        QStandardItem* item = new NumorItem(exp, data_ptr);
        item->setText(QString::fromStdString(basename));
        item->setCheckable(true);
        dataItem->appendRow(item);

    }
}



void ExperimentTree::findPeaks(const QModelIndex& index)
{

    MainWindow* main=dynamic_cast<MainWindow*>(QApplication::activeWindow());

    auto ui=main->getUI();

    ui->_dview->getScene()->clearPeaks();

    QStandardItem* item=_model->itemFromIndex(index);

    TreeItem* titem=dynamic_cast<TreeItem*>(item);
    if (!titem)
        return;

    std::shared_ptr<SX::Instrument::Experiment> expt(titem->getExperiment());

    if (!expt)
        return;

    QStandardItem* ditem=_model->itemFromIndex(index);

    std::vector<std::shared_ptr<SX::Data::IData>> selectedNumors;
    int nTotalNumors(_model->rowCount(ditem->index()));
    selectedNumors.reserve(nTotalNumors);

    for (auto i=0;i<nTotalNumors;++i)
    {
        if (ditem->child(i)->checkState() == Qt::Checked)
        {
            if (auto ptr = dynamic_cast<NumorItem*>(ditem->child(i)))
                selectedNumors.push_back(ptr->getExperiment()->getData(ptr->text().toStdString()));
        }
    }

    if (selectedNumors.empty())
    {
        qWarning()<<"No numors selected for finding peaks";
        return;
    }

    // run peak find
    auto frame = ui->_dview->getScene()->getCurrentFrame();

    // check if now frame was loaded, or is otherwise invalid
    if ( frame.rows() == 0 || frame.cols() == 0) {
        // attempt to read first frame of first numor by default
        try {
            selectedNumors[0]->open();
            frame = selectedNumors[0]->getFrame(0);
        }
        catch(std::exception& e) {
            qDebug() << "Peak search failed: cannot load frame: " << e.what();
            return;
        }
    }

    qDebug() << "Preview frame has dimensions" << frame.rows() << " " << frame.cols();

    DialogConvolve* dialog = new DialogConvolve(frame, this);

    // dialog will automatically be deleted before we return from this method
    std::unique_ptr<DialogConvolve> dialog_ptr(dialog);



    // reset progress handler
    _progressHandler = std::shared_ptr<ProgressHandler>(new ProgressHandler);

    // set up peak finder
    if ( !_peakFinder)
        _peakFinder = std::shared_ptr<PeakFinder>(new PeakFinder);
    _peakFinder->setHandler(_progressHandler);

    // dialog will be initialized with values from current peak finder,
    // and any changes made will persist
    dialog->setPeakFinder(_peakFinder);

    if (!dialog->exec())
        return;

    int max=selectedNumors.size();
    qWarning() << "Peak find algorithm: Searching peaks in " << max << " files";
    
    // create a pop-up window that will show the progress
    ProgressView* progressView = new ProgressView(this);
    progressView->watch(_progressHandler);

    // lambda function to execute peak search in a separate thread
    auto task = [=] () -> bool
    {
        bool result = false;

        // execute in a try-block because the progress handler may throw if it is aborted by GUI
        try {
            result = _peakFinder->find(selectedNumors);
        }
        catch(std::exception& e) {
            qDebug() << "Caught exception during peak find: " << e.what();
            qDebug() <<" Peak search aborted.";
            return false;
        }
        return result;
    };

    auto onFinished = [=] (bool succeeded) -> void
    {
        // delete the progressView
        delete progressView;

        int num_peaks = 0;

        for (auto numor: selectedNumors) {
            num_peaks += numor->getPeaks().size();
            numor->releaseMemory();
        }

        if ( succeeded ) {
            ui->_dview->getScene()->updatePeaks();

            qDebug() << "Peak search complete., found "
                     << num_peaks
                     << " peaks.";
        }
        else {
            qDebug() << "Peak search failed!";
        }
    };

    auto job = new Job(this, task, onFinished, true);
    //connect(progressView, SIGNAL(canceled()), job, SLOT(terminate()));

    job->exec();
}

void ExperimentTree::viewReciprocalSpace(const QModelIndex& index)
{
    QStandardItem* item=_model->itemFromIndex(index);

    TreeItem* titem=dynamic_cast<TreeItem*>(item);
    if (!titem)
        return;

    std::shared_ptr<SX::Instrument::Experiment> expt(titem->getExperiment());

    if (!expt)
        return;

    QStandardItem* ditem=_model->itemFromIndex(index);

    std::vector<std::shared_ptr<SX::Data::IData>> selectedNumors;
    int nTotalNumors(_model->rowCount(ditem->index()));
    selectedNumors.reserve(nTotalNumors);

    for (auto i=0;i<nTotalNumors;++i)
    {
        if (ditem->child(i)->checkState() == Qt::Checked)
        {
            if (auto ptr = dynamic_cast<NumorItem*>(ditem->child(i)))
                selectedNumors.push_back(ptr->getExperiment()->getData(ptr->text().toStdString()));
        }
    }

    if (selectedNumors.empty())
    {
        qWarning()<<"No numor selected for reciprocal viewer";
        return;
    }

    try
    {
        ReciprocalSpaceViewer* dialog = new ReciprocalSpaceViewer(expt);
        dialog->setData(selectedNumors);
        if (!dialog->exec())
            return;
    }
    catch(std::exception& e)
    {
        qWarning()<<e.what();
        return;
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
        std::shared_ptr<SX::Instrument::Experiment> exp = ptr->getExperiment();
        emit plotData(exp->getData(item->text().toStdString()));
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


// jmf: this method no longer used?
//ExperimentItem* ExperimentTree::getExperimentItem(SX::Instrument::Experiment* exp)
//{

//    QModelIndex rootIdx = rootIndex();

//    for (auto i=0;i<_model->rowCount(rootIdx);++i)
//    {
//        auto idx = _model->index(i,0,rootIdx);
//        auto ptr=dynamic_cast<ExperimentItem*>(_model->itemFromIndex(idx));
//        if (ptr && ptr->getExperiment().get()==exp)
//            return ptr;
//    }

//    return nullptr;

// }

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

void ExperimentTree::showPeaksOpenGL()
{
    GLWidget* glw=new GLWidget();
    auto& scene=glw->getScene();
    auto datav=getSelectedNumors();
    for (auto idata : datav)
    {
       auto peaks=idata->getPeaks();
       for (auto peak: peaks)
       {
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
    SpaceGroupDialog* dialog = new SpaceGroupDialog(getSelectedNumors(), this);
    dialog->exec();
    // update the space group elsewhere
}

void ExperimentTree::computeRFactors()
{
    qDebug() << "Finding peak equivalences...";

    std::vector<std::shared_ptr<IData>> numors = getSelectedNumors();
    std::vector<std::vector<Peak3D*>> peak_equivs;
    std::vector<Peak3D*> peak_list;

    std::shared_ptr<UnitCell> unit_cell;

    for (std::shared_ptr<IData> numor: numors) {
        std::set<Peak3D*> peaks = numor->getPeaks();
        for (Peak3D* peak: peaks)
            if ( peak && peak->isSelected() && !peak->isMasked() )
                peak_list.push_back(peak);
    }

    if ( peak_list.size() == 0) {
        qDebug() << "No peaks -- cannot search for equivalences!";
        return;
    }

    for (Peak3D* peak: peak_list) {
        // what do we do if there is more than one sample/unit cell??
        unit_cell = peak->getUnitCell();

        if (unit_cell)
            break;
    }

    if (!unit_cell) {
        qDebug() << "No unit cell selected! Cannot compute R factors.";
        return;
    }

    SpaceGroup grp("P 1");

    // spacegroup construct can throw
    try {
        grp = SpaceGroup(unit_cell->getSpaceGroup());
    }
    catch(std::exception& e) {
        qDebug() << "Caught exception: " << e.what() << endl;
        return;
    }

    peak_equivs = grp.findEquivalences(peak_list, true);

    qDebug() << "Found " << peak_equivs.size() << " equivalence classes of peaks:";

    std::map<int, int> size_counts;

    for (auto& peaks: peak_equivs) {
        ++size_counts[peaks.size()];
    }

    for (auto& it: size_counts) {
        qDebug() << "Found " << it.second << " classes of size " << it.first;
    }

    qDebug() << "Computing R factors:";

    RFactor rfactor(peak_equivs);

    qDebug() << "    Rmerge = " << rfactor.Rmerge();
    qDebug() << "    Rmeas  = " << rfactor.Rmeas();
    qDebug() << "    Rpim   = " << rfactor.Rpim();

    //ScaleDialog* scaleDialog = new ScaleDialog(peak_equivs, this);
    //scaleDialog->exec();
}

void ExperimentTree::findFriedelPairs()
{
    qDebug() << "findFriedelParis() is not yet implemented!";
    return;

//    std::vector<Peak3D*> peaks;
//    std::vector<std::shared_ptr<IData>> numors = getSelectedNumors();

//    for (std::shared_ptr<IData> numor: numors) {
//        std::set<Peak3D*> peak_list = numor->getPeaks();

//        for (Peak3D* peak: peak_list)
//            peaks.push_back(peak);
//    }



    // todo: something with FriedelDialog!
    //FriedelDialog* friedelDialog = new FriedelDialog(peaks, this);
    //friedelDialog->exec();
    //delete friedelDialog;
}
