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

typedef struct {
    double r,g,b;
} COLOUR;

COLOUR GetColour(double v,double vmin,double vmax)
{
   COLOUR c = {1.0,1.0,1.0}; // white
   double dv;

   if (v < vmin)
      v = vmin;
   if (v > vmax)
      v = vmax;
   dv = vmax - vmin;

   if (v < (vmin + 0.4 * dv)) {
      c.r = 2.5*(v-vmin) / dv;
      c.g = 0;
      c.b = 0;
   } else if (v < (vmin + 0.8 * dv)) {
      c.r = 1;
      c.g = 1 + 2.5 * (-vmin - 0.4 * dv + v) / dv;
      c.b=  0.;
   } else {
      c.r = 1;
      c.g = 1;
      c.b = 1 + 4 * (-vmin - 0.8 * dv +v) / dv;
    }
   return c;
}

QImage Mat2QImage(int* src, int rows, int cols,double max=10.0)
{
        QImage dest(cols, rows, QImage::Format_RGB32);
        for (int y = 0; y < rows; ++y) {
            const int* srcrow = src+y;
                QRgb *destrow = reinterpret_cast<QRgb*>(dest.scanLine(y));
                for (int x = 0; x < cols; ++x) {
                        double temp=*(srcrow+x*rows);
                        COLOUR cc=GetColour(temp,0.0,max);
                        destrow[x] = qRgb(cc.r*255,cc.g*255,cc.b*255);
                }
        }
        return dest;
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),scene(new QGraphicsScene)
{
    ui->setupUi(this);
    scene->setParent(ui->_dview);
    ui->_dview->setScene(scene);
    ui->_dview->setInteractive(true);
    _blur=new QGraphicsBlurEffect();
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

           ui->textLogger->log(Logger::INFO) << "File: " << fileNames[i].toStdString() << " is not readable as a Numor";
           ui->textLogger->flush();
           ui->textLogger->update();
           continue;
        }
        ui->numor_Widget->addItem(litem);
        }
    }
    QListWidgetItem* first=ui->numor_Widget->item(0);

    if (!first)
        return;

    ui->textLogger->log(Logger::INFO) << "Read " << fileNames.size() << " files"<<std::endl;
    ui->textLogger->flush();

    ui->numor_Widget->setCurrentItem(first);
    std::string firstNumor = first->text().toStdString();
    int b=_data[firstNumor]._nblocks;
    ui->horizontalScrollBar->setRange(0,b-1);
    ui->dial->setRange(1,100);
    ui->dial->setValue(10);    update();
    QImage image=QImage(Mat2QImage(&((_data[firstNumor]._frames)[0]),256,640,ui->dial->value()));
    QPixmap pix=QPixmap::fromImage(image);
    pix=pix.scaled(ui->_dview->width(),ui->_dview->height(),Qt::IgnoreAspectRatio);
    scene->clear();
    scene->addPixmap(pix);

//    QGraphicsEllipseItem* el1=new QGraphicsEllipseItem(50,50,10,20);
//    el1->setToolTip(QString("(1,0,2) \n I: 123(4)"));
//    el1->setRotation(20.0);
//    scene->addItem(el1);
//    el1->setPos(50,50);
//    el1->setRotation(20.0);
//    el1->setPen(QPen(QColor(Qt::white)));
//    el1->setFlag(QGraphicsItem::ItemIsSelectable);
//    el1->setSelected(true);
//    el1->update();

//    QGraphicsEllipseItem* el2=new QGraphicsElli    update();pseItem(100,100,40,40);
//    el2->setRotation(20.0);
//    scene->addItem(el2);value_typ
//    el2->setPos(50,50);
//    el2->setRotation(20.0);
//    el2->setPen(QPen(QColor(Qt::white)));
//    el2->setFlag(QGraphicsItem::ItemIsSelectable);
//    el2->setSelected(true);
//    el2->update();
    ui->_dview->setScene(scene);

}

void MainWindow::on_horizontalScrollBar_valueChanged(int value)
{
    if (_data.size()==0)
        return;
    QListWidgetItem* item=ui->numor_Widget->currentItem();
    std::string numor=item->text().toStdString();
    _data[numor].readBlock(value);
    updatePlot(numor);
}

void MainWindow::on_dial_valueChanged(int value)
{
    if (_data.size()==0)
        return;
    QListWidgetItem* item=ui->numor_Widget->currentItem();
    std::string numor=item->text().toStdString();
    ui->horizontalScrollBar->setMouseTracking(true);
    updatePlot(numor);
}

void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
  _blur->setBlurRadius(arg1);
    ui->_dview->setGraphicsEffect(_blur);
}

void MainWindow::on_numor_Widget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    ui->numor_Widget->setSelectionMode(QAbstractItemView::MultiSelection);
    if (current==previous || current == nullptr)
        return;
    std::string numor=current->text().toStdString();
    int nmax=_data[numor]._nblocks-1;
    ui->horizontalScrollBar->setMaximum(nmax);
    ui->spinBox_Frame->setMaximum(nmax);
    updatePlot(numor);

}

void MainWindow::updatePlot(const std::string& numor)
{
    auto it=_data.find(numor);
    if (it!=_data.end())
    {
    QImage image=QImage(Mat2QImage(&(((*it).second._frames)[0]),256,640,ui->dial->value()));
    QPixmap pix=QPixmap::fromImage(image);
    pix=pix.scaled(ui->_dview->width(),ui->_dview->height(),Qt::IgnoreAspectRatio);
    scene->clear();
    scene->addPixmap(pix);
    ui->_dview->setScene(scene);
    }
    else
    {
        scene->clear();
//        ui->_dview->setScene(scene);
    }
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
    updatePlot(std::string());
    ui->textLogger->log(Logger::INFO) <<  selNumors.size() << " file(s) have been deleted\n";
    ui->textLogger->flush();

}


void on_pushButton_PeakFind_clicked();

void MainWindow::on_spinBox_max_valueChanged(int arg1)
{
    ui->dial->setValue(arg1);
     QListWidgetItem* item=ui->numor_Widget->currentItem();
    if (item)
    {
    std::string numor=item->text().toStdString();
    updatePlot(numor);
    }
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
    updatePlot(numor);
    ui->textLogger->log(Logger::INFO) <<  "File " << numor << " selected \n";
    ui->textLogger->flush();
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
    updatePlot(numor);

}



void MainWindow::ShowContextMenu(const QPoint& pos)
{
    loggerContextMenu ->exec(QCursor::pos());
}


void MainWindow::on_pushButton_PeakFind_clicked()
{
    DialogPeakFind* dialog= new DialogPeakFind();

    //
    if (!dialog->exec())
        return;
    double confidence=dialog->getConfidence();
    double threshold=dialog->getThreshold();

    QListWidgetItem* current=ui->numor_Widget->currentItem();
    if (!current)
        return;
    std::string numor=current->text().toStdString();
    auto it=_data.find(numor);
    if (it!=_data.end())
    {
        Data& d=(*it).second;
        d.readInMemory();
        std::vector<int*> temp(d._nblocks);
        for (int i=0;i<d._nblocks;++i)
        {
            vint& v=d._data[i];
            temp[i]=&(v[0]);
        }
        SX::Geometry::blob3DCollection blobs;
        try
        {
            blobs=std::move(SX::Geometry::findBlobs3D<int>(temp, 256,640, threshold, 5, 10000, confidence, 0));
        }catch(std::bad_alloc& e)
        {
            std::cout << "Bad alloc detected " << std::endl;
        }

        //std::cout << "Found" << blobs.size() << " peaks " << std::endl;
        //std::for_each(blobs.begin(),blobs.end(),
        //              [](std::pair<int,SX::Geometry::Blob3D> b){ std::cout << b.second << std::endl;});
        // Now free the memory
        (*it).second.releaseMemory();
    }
}

void MainWindow::on_textLogger_customContextMenuRequested(const QPoint &pos)
{
    std::cout  << pos.x() << std::endl;
}


