#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cstdlib>
#include <QFileDialog>
#include <QShortcut>
#include <QThread>
#include <QGraphicsEllipseItem>
#include <QGraphicsBlurEffect>
#include <QMessageBox>
#include <QMouseEvent>
#include "BlobFinder.h"
#include <Ellipsoid.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),scene(new QGraphicsScene)
{
    ui->setupUi(this);
    scene->setParent(ui->_dview);
    ui->_dview->setScene(scene);
    ui->_dview->setInteractive(true);
    ui->_dview->setDragMode(QGraphicsView::RubberBandDrag);
    //ui->_dview->setDragMode(ui->_dview->ScrollHandDrag);
    ui->dial->setRange(0,15);
    ui->pushButton_openFile->setIcon(QIcon("/home/chapon/Programming/C++/Qt/D19Proto/D19Proto/IconopenFile.jpg"));
    ui->pushButton_openFile->setIconSize(QSize(55, 55));

    ui->pushButton_PeakFind->setIcon(QIcon("/home/chapon/Programming/C++/Qt/D19Proto/D19Proto/IconfindPeak.jpg"));
    ui->pushButton_PeakFind->setIconSize(QSize(55, 55));
    ui->numor_Widget->setSelectionMode(QAbstractItemView::MultiSelection);
    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->numor_Widget);
    connect(shortcut, SIGNAL(activated()), this, SLOT(deleteNumors()));
    ui->progressBar->setVisible(false);
//    ui->textLogger->setContextMenuPolicy(Qt::CustomContextMenu);
//    loggerContextMenu = ui->textLogger->createStandardContextMenu();
//    loggerContextMenu->clear();
//    loggerContextMenu ->addAction("Clear LogFile");
//    loggerContextMenu->addSeparator();
//    loggerContextMenu ->addAction("Save LogFile");

//    ui->textLogger->setContextMenuPolicy(Qt::CustomContextMenu);

//    connect(ui->textLogger, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(ShowContextMenu(const QPoint&)));
    ui->_dview->setDimensions(640,256);

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_action_Open_triggered()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFiles);
    QStringList fileNames;
    fileNames= dialog.getOpenFileNames(this);
    dialog.close();
    for (int i=0;i<fileNames.size();++i)
    {
        QFileInfo fileinfo(fileNames[i]);
        QString numor = fileinfo.baseName(); // Return only a file name
        std::string index=numor.toStdString();
        auto it=_data.find(index);
        if (it==_data.end()) // Add numor only if not already in the list
        {
        QListWidgetItem* litem=new QListWidgetItem(numor);
        _data.insert(std::unordered_map<std::string,Data>::value_type(index,Data()));
        try
        {
            _data[index].fromFile(fileNames[i].toStdString());
        }catch(...)
        {

           ui->textLogger->log(Logger::WARNING) << "File: " << fileNames[i].toStdString() << " is not readable as a Numor";
           ui->textLogger->flush();

           continue;
        }
        ui->numor_Widget->addItem(litem);
        }
    }
    QListWidgetItem* first=ui->numor_Widget->item(0);

    if (!first)
        return;

    ui->textLogger->log(Logger::INFO) << "Read " << fileNames.size() << " file(s)";
    ui->textLogger->flush();

    ui->numor_Widget->setCurrentItem(first);
    std::string firstNumor = first->text().toStdString();
    int b=_data[firstNumor]._nblocks;
    ui->horizontalScrollBar->setRange(0,b-1);
    ui->dial->setRange(1,100);
    ui->dial->setValue(10);
    updatePlot();
}

void MainWindow::updatePlot()
{
    QListWidgetItem* item=ui->numor_Widget->currentItem();
    std::string numor=item->text().toStdString();
    auto it=_data.find(numor);
    if (it!=_data.end())
    {
        Data* ptr=&(it->second);
        int frame= ui->horizontalScrollBar->value();
        int colormax= ui->dial->value();
        ui->_dview->updateView(ptr,frame,colormax);
    }
    else
        ui->_dview->updateView(0,0,0);
}

void MainWindow::on_horizontalScrollBar_valueChanged(int value)
{
    updatePlot();
}

void MainWindow::on_dial_valueChanged(int value)
{
    updatePlot();
}


void MainWindow::on_pushButton_openFile_pressed()
{
    on_action_Open_triggered();
}

void MainWindow::deleteNumors()
{
    QList<QListWidgetItem*> selNumors = ui->numor_Widget->selectedItems();

    for (auto it=selNumors.begin();it!=selNumors.end();++it)
    {
        auto it1 = _data.find((*it)->text().toStdString());
        if (it1 != _data.end())
            _data.erase(it1);
        delete *it;
    }
    ui->textLogger->log(Logger::INFO) <<  selNumors.size() << " file(s) have been deleted";
    ui->textLogger->flush();
    updatePlot();

}


void MainWindow::on_spinBox_max_valueChanged(int arg1)
{
    ui->dial->setValue(arg1);
    updatePlot();
}


void MainWindow::on_numor_Widget_itemDoubleClicked(QListWidgetItem *item)
{
    ui->numor_Widget->clearSelection();
    ui->numor_Widget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->numor_Widget->setItemSelected(item,true);
    if (!item)
       return;
    std::string numor=item->text().toStdString();
    int nmax=_data[numor]._nblocks-1;
    ui->horizontalScrollBar->setMaximum(nmax);
    ui->spinBox_Frame->setMaximum(nmax);
    ui->textLogger->log(Logger::INFO) <<  "File " << numor << " selected ";
    ui->textLogger->flush();
    updatePlot();
}

void MainWindow::on_numor_Widget_itemActivated(QListWidgetItem *item)
{
    ui->numor_Widget->setSelectionMode(QAbstractItemView::MultiSelection);
    if (item == nullptr)
        return;
    std::string numor=item->text().toStdString();
    ui->textLogger->flush();
    int nmax=_data[numor]._nblocks-1;
    ui->horizontalScrollBar->setMaximum(nmax);
    ui->spinBox_Frame->setMaximum(nmax);
    updatePlot();

}



void MainWindow::ShowContextMenu(const QPoint& pos)
{
    loggerContextMenu ->exec(QCursor::pos());
}


void MainWindow::on_pushButton_PeakFind_clicked()
{
    DialogPeakFind* dialog= new DialogPeakFind(this);

    //
    if (!dialog->exec())
        return;

    // Get Confidence and threshold
    double confidence=dialog->getConfidence();
    double threshold=dialog->getThreshold();

    // Get current Numor
    QListWidgetItem* current=ui->numor_Widget->currentItem();
    if (!current)
        return;

    //
    std::string numor=current->text().toStdString();
    ui->textLogger->log(Logger::INFO) << "Searching peaks in numor:" << numor;
    ui->textLogger->flush();

    // Get the corresponding data
    auto it=_data.find(numor);
    if (it==_data.end())
        return;

    Data& d=(*it).second;
    ui->textLogger->log(Logger::INFO) << "Reading numor in memory";
    ui->textLogger->flush();
    ui->progressBar->setVisible(true);
    d.readInMemory(ui->progressBar);


    // Analyse background
    std::vector<int> v;
    d.getCountsHistogram(v);
    auto max=std::max_element(v.begin(),v.end());
    int background_level=std::distance(v.begin(),max)+1;

    ui->textLogger->log(Logger::INFO) << "Background estimated at "  << background_level << "cts/pixel";
    ui->textLogger->flush();

    // Get pointers to start of each frame
    std::vector<int*> temp(d._nblocks);
    for (int i=0;i<d._nblocks;++i)
    {
        vint& v=d._data[i];
        temp[i]=&(v[0]);
    }


    // Finding peaks
    SX::Geometry::blob3DCollection blobs;
    ui->progressBar->setFormat("Locating peaks...");
    try
    {
      blobs=SX::Geometry::findBlobs3D<int>(temp, 256,640, threshold*background_level, 5, 10000, confidence, 0);
    }catch(std::bad_alloc& e) // Warning if RAM error
    {
        ui->textLogger->log(Logger::WARNING) << e.what() << "(Peak find, memory allocation error. Increase Threshold level)";
        ui->textLogger->flush();
    }

    ui->textLogger->log(Logger::INFO) << "Found: " << blobs.size() << " peaks";
    ui->textLogger->flush();

    // Now free the memory
    d.releaseMemory();
    ui->progressBar->setVisible(false);

    //
    int i=0;
    for(auto& blob : blobs)
    {
        double confidence;
        Eigen::Vector3d center, eigenvalues;
        Eigen::Matrix3d eigenvectors;
        blob.second.toEllipsoid(confidence, center,eigenvalues,eigenvectors);
        SX::Geometry::Ellipsoid<double,3> a(center,eigenvalues,eigenvectors);
        d._peaks[i++]=a;
    }
}

void MainWindow::on_textLogger_customContextMenuRequested(const QPoint &pos)
{
    std::cout  << pos.x() << std::endl;
}


