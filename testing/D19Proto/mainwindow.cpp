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
#include <Plotter1D.h>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),scene(new QGraphicsScene)
{
    ui->setupUi(this);

    ui->mainToolBar->setIconSize(QSize(32,32));

    scene->setParent(ui->_dview);
    ui->_dview->setScene(scene);
    ui->_dview->setInteractive(true);
    ui->_dview->setDragMode(QGraphicsView::RubberBandDrag);
    //ui->_dview->setDragMode(ui->_dview->ScrollHandDrag);
    ui->dial->setRange(0,15);

    ui->numor_Widget->setSelectionMode(QAbstractItemView::MultiSelection);
    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->numor_Widget);
    connect(shortcut, SIGNAL(activated()), this, SLOT(deleteNumors()));
    ui->progressBar->setVisible(false);

    // Specific to D19 detector
    ui->_dview->setNpixels(640,256);
    ui->_dview->setDimensions(120.0,0.4);
    ui->_dview->setDetectorDistance(0.764);
    //

    ui->selectionMode->addItem(QIcon(":/zoomIcon.png"),"");
    ui->selectionMode->addItem(QIcon(":/cutlineIcon.png"),"");
    ui->selectionMode->addItem(QIcon(":/cutellipseIcon.png"),"");
    ui->selectionMode->addItem(QIcon(":/horizontalsliceIcon.png"),"");
    ui->selectionMode->addItem(QIcon(":/verticalsliceIcon.png"),"");

    connect(ui->selectionMode,SIGNAL(currentIndexChanged(int)),ui->_dview,SLOT(setCutterMode(int)));
    connect(ui->dial,SIGNAL(valueChanged(int)),ui->_dview,SLOT(setMaxIntensity(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_action_open_triggered()
{
    QFileDialog dialog;
    dialog.setDirectory("/home/chapon/Data/D19/July2014/data/DKDP/");
    dialog.setFileMode(QFileDialog::ExistingFiles);
    QStringList fileNames;
    fileNames= dialog.getOpenFileNames(this,"select numors","/home/chapon/Data/D19/July2014/data/DKDP/");
    // No files selected
    if (fileNames.isEmpty())
        return;
    // Deal with files
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

           qDebug() << "File: " << fileNames[i] << " is not readable as a Numor";

           continue;
        }
        ui->numor_Widget->addItem(litem);
        }
    }
    QListWidgetItem* first=ui->numor_Widget->item(0);

    if (!first)
        return;

    qDebug() << "Read " << fileNames.size() << " file(s)";


    ui->numor_Widget->setCurrentItem(first);
    std::string firstNumor = first->text().toStdString();
    Data& d=_data[firstNumor];
    ui->horizontalScrollBar->setMaximum(d._nblocks-1);
    ui->spinBox_Frame->setMaximum(d._nblocks-1);
    ui->dial->setRange(1,3000);
    ui->dial->setValue(20);
    updatePlot();
}

void MainWindow::updatePlot()
{
    // Get the numor currently selected, return if void
    QListWidgetItem* item=ui->numor_Widget->currentItem();
    if (!item)
    {
        ui->_dview->updateView(0,0);
        return;
    }
    // Update the data in the detector view
    std::string numor=item->text().toStdString();
    auto it=_data.find(numor);
    if (it!=_data.end())
    {
        Data* ptr=&(it->second);
        int frame= ui->horizontalScrollBar->value();
        // Make sure that the frame value for previously selected file is valid for current one
        if (frame>ptr->_nblocks-1)
            frame=0;
        // Keep previous color
        int colormax= ui->dial->value();
        ui->_dview->updateView(ptr,frame);
    }

}

void MainWindow::on_horizontalScrollBar_valueChanged(int value)
{
    updatePlot();
}

void MainWindow::on_dial_valueChanged(int value)
{
    updatePlot();
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
    qDebug() <<  selNumors.size() << " file(s) have been deleted";

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


    updatePlot();
}

void MainWindow::on_numor_Widget_itemActivated(QListWidgetItem *item)
{
    ui->numor_Widget->setSelectionMode(QAbstractItemView::MultiSelection);
    if (item == nullptr)
        return;
    std::string numor=item->text().toStdString();

    int nmax=_data[numor]._nblocks-1;
    ui->horizontalScrollBar->setMaximum(nmax);
    ui->spinBox_Frame->setMaximum(nmax);
    updatePlot();

}



void MainWindow::ShowContextMenu(const QPoint& pos)
{
    loggerContextMenu ->exec(QCursor::pos());
}


void MainWindow::on_action_peak_find_triggered()
{
    DialogPeakFind* dialog= new DialogPeakFind();

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

    // Get the corresponding data
    auto it=_data.find(numor);
    if (it==_data.end())
        return;

    setCursor(Qt::WaitCursor);




    Data& d=(*it).second;



    ui->progressBar->setVisible(true);

    d.readInMemory();


    // Analyse background
//    std::vector<int> v;
//    d.getCountsHistogram(v);

//    auto max=std::max_element(v.begin(),v.end());
//    int background_level=std::distance(v.begin(),max)+1;
//    std::cout << v[0] << std::endl;
//    qDebug() << "Background estimated at "  << background_level << "cts/pixel";


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
      blobs=SX::Geometry::findBlobs3D<int>(temp, 256,640, threshold*3, 5, 10000, confidence, 0);
    }catch(std::exception& e) // Warning if RAM error
    {


    }



    // Now free the memory
    d.releaseMemory();
    ui->progressBar->setVisible(false);

    //
    int i=0;
    d._peaks.clear();
    for (auto& blob : blobs)
    {
        Eigen::Vector3d center, eigenvalues;
        Eigen::Matrix3d eigenvectors;
        blob.second.toEllipsoid(confidence, center,eigenvalues,eigenvectors);
        SX::Geometry::Ellipsoid<double,3> a(center,eigenvalues,eigenvectors);
        d._peaks[i++]=a;
    }


    //
    setCursor(Qt::ArrowCursor);

    std::cout << "J ai fini et j ai trouve" << blobs.size() <<  std::endl;
    updatePlot();
}

void MainWindow::on_textLogger_customContextMenuRequested(const QPoint &pos)
{
    std::cout  << pos.x() << std::endl;
}


