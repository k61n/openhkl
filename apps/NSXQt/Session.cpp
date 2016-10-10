/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forshungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "ui_MainWindow.h"
#include "DialogConvolve.h"
#include "ProgressHandler.h"
#include "ProgressView.h"

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
#include "Session.h"
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
#include <hdf5.h>
#include <H5Exception.h>

#include "PeakFitDialog.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>


using std::vector;
using SX::Data::IData;
using std::shared_ptr;
using SX::Utils::ProgressHandler;

Session::Session(QWidget *parent) : QObject(parent)
{


    _model=new QStandardItemModel(this);




    connect(this,SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onCustomMenuRequested(const QPoint&)));
    connect(this,SIGNAL(doubleClicked(const QModelIndex&)),this,SLOT(onDoubleClick(const QModelIndex&)));
    connect(this,SIGNAL(clicked(QModelIndex)),this,SLOT(onSingleClick(QModelIndex)));
}

Session::~Session()
{
    // _model should be deleted automatically during destructor by QT
    //delete _model;
}

void Session::createNewExperiment()
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

void Session::addExperiment(const std::string& experimentName, const std::string& instrumentName)
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


}

vector<shared_ptr<IData>> Session::getSelectedNumors(ExperimentItem* item) const
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

vector<shared_ptr<IData>> Session::getSelectedNumors() const
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


void Session::absorptionCorrection()
{
    // NOT IMPLEMENTED
}

void Session::importData()
{
    // NOT IMPLEMENTED
}



void Session::findPeaks(const QModelIndex& index)
{
    // NOT IMPLEMENTED
}



void Session::showPeaksOpenGL()
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

void Session::findSpaceGroup()
{
    // NOT IMPLEMENTED
}

void Session::computeRFactors()
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

void Session::findFriedelPairs()
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

void Session::integrateCalculatedPeaks()
{
    qDebug() << "Integrating calculated peaks...";

    int count = 0;
    Eigen::Vector3d peak_extent, bg_extent;
    peak_extent << 0.0, 0.0, 0.0;
    bg_extent << 0.0, 0.0, 0.0;

    std::shared_ptr<UnitCell> unit_cell;

    for (std::shared_ptr<IData> numor: getSelectedNumors()) {
        for (Peak3D* peak: numor->getPeaks())
            if ( peak && peak->isSelected() && !peak->isMasked() ) {
                peak_extent += peak->getPeak()->getAABBExtents();
                bg_extent += peak->getBackground()->getAABBExtents();
                ++count;
            }
    }

    if ( count == 0) {
        qDebug() << "No peaks -- cannot search for equivalences!";
        return;
    }

    peak_extent /= count;
    bg_extent /= count;

    qDebug() << "Done calculating average bounding box";

    qDebug() << peak_extent(0) << " " << peak_extent(1) << " " << peak_extent(2);
    qDebug() << bg_extent(0) << " " << bg_extent(1) << " " << bg_extent(2);

    for (std::shared_ptr<IData> numor: getSelectedNumors()) {



        std::vector<Peak3D*> calculated_peaks;

        shared_ptr<Sample> sample = numor->getDiffractometer()->getSample();
        int ncrystals = sample->getNCrystals();

        if (ncrystals) {
            for (int i = 0; i < ncrystals; ++i) {
                SX::Crystal::SpaceGroup group(sample->getUnitCell(i)->getSpaceGroup());
                auto ub = sample->getUnitCell(i)->getReciprocalStandardM();

                qDebug() << "Calculating peak locations...";

                auto hkls = sample->getUnitCell(i)->generateReflectionsInSphere(1.5);
                std::vector<SX::Crystal::PeakCalc> peaks = numor->hasPeaks(hkls, ub);
                calculated_peaks.reserve(calculated_peaks.size() + peaks.size());

                qDebug() << "Adding calculated peaks...";

                for(auto&& p: peaks) {
                    //calculated_peaks.push_back(p);
                }
            }
        }

        qDebug() << "Done.";
    }
}

void Session::peakFitDialog()
{
    // NOT IMPLEMENTED
}

void Session::incorporateCalculatedPeaks()
{
    // NOT IMPLEMENTED
}
