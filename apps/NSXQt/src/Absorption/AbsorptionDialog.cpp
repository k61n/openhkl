#include "include/Absorption/AbsorptionDialog.h"
#include "ui_AbsorptionDialog.h"
#include <fstream>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
#include "Diffractometer.h"
#include "Sample.h"
#include "Experiment.h"
#include "RotAxis.h"
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <sstream>
#include "Gonio.h"
#include <QtDebug>
#include <QFileInfo>
#include <QDir>

AbsorptionDialog::AbsorptionDialog(SX::Instrument::Experiment* experiment,QWidget *parent) :
    QDialog(parent),ui(new Ui::AbsorptionDialog),_experiment(experiment),_cscene(new CrystalScene(&experiment->getDiffractometer()->getSample()->getShape()))
{
    ui->setupUi(this);

    ui->graphicsView->setScene(_cscene);
    //
    connect(this,SIGNAL(loadImage(QString)),_cscene,SLOT(loadImage(QString)));
    // Horizontal slider changes image of the movie in the scene
    connect(ui->horizontalScrollBar,&QScrollBar::valueChanged,[=](int i){_cscene->loadImage(QString::fromStdString(_imageList[i].second));});
    connect(ui->horizontalScrollBar,&QScrollBar::valueChanged,[=](int i){_cscene->drawText("Phi: "+QString::number(_imageList[i].first));
    connect(ui->horizontalScrollBar,&QScrollBar::valueChanged,[=](int i){_cscene->setRotationAngle(_imageList[i].first);});
    });

    //
    connect(ui->button_calibrateDistance,SIGNAL(pressed()),_cscene,SLOT(activateCalibrateDistance()));
    connect(ui->button_pickCenter,SIGNAL(pressed()),_cscene,SLOT(activatePickCenter()));
    connect(ui->button_pickingPoints,SIGNAL(pressed()),_cscene,SLOT(activatePickingPoints()));
    connect(ui->button_removingPoints,SIGNAL(pressed()),_cscene,SLOT(activateRemovingPoints()));
    connect(ui->button_triangulate,SIGNAL(pressed()),_cscene,SLOT(triangulate()));

    connect(_cscene,&CrystalScene::calibrateDistanceOK,[=](){ui->button_pickCenter->setEnabled(true);});
    connect(_cscene,&CrystalScene::calibrateCenterOK,[=](){ui->button_pickingPoints->setEnabled(true);});
    connect(_cscene,&CrystalScene::calibrateCenterOK,[=](){ui->button_removingPoints->setEnabled(true);});
    connect(_cscene,&CrystalScene::calibrateCenterOK,[=](){ui->button_triangulate->setEnabled(true);});

    connect(this,SIGNAL(angleText(QString)),_cscene,SLOT(drawText(QString)));

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    setupInitialButtons();
}

AbsorptionDialog::~AbsorptionDialog()
{
    delete ui;
}

const std::string& AbsorptionDialog::getMovieFilename() const
{
    return _filepath;
}

void AbsorptionDialog::readInfoFile(const std::string &filename)
{
    // Clear alll images.
    _imageList.clear();
    _imageList.shrink_to_fit();

    std::ifstream file;
    file.open(filename.c_str(),std::ios::in);

    if (file.is_open())
    {
        // First read instrument name and validate with diffractometer name
        std::string _instrumentName, date;
        file >> _instrumentName >> date;
        if (_instrumentName.compare(_experiment->getDiffractometer()->getType())!=0)
        {
            QMessageBox::critical(this, tr("NSXTool"),
                                  tr("Instrument name in video file does not match the diffractometer name"));
        }

        std::string line;
        // Skip one line (comment)
        getline(file,line);
        getline(file,line);
        // Read line with goniometer angles
        getline(file,line);
        // Cout number of axes, validate with goniometer definition
        std::size_t numberAngles=std::count(line.begin(),line.end(),':');
        std::size_t sampleAngles=_experiment->getDiffractometer()->getSample()->getNPhysicalAxes();
        if (numberAngles==0 || (numberAngles!=sampleAngles))
            QMessageBox::critical(this, tr("NSXTool"),
                                  tr("Number of goniometer axes in video file do not match instrument definition"));
        // Remove all occurences of ':' before reading
        line.erase(std::remove(line.begin(), line.end(), ':'), line.end());
        std::stringstream is(line);
        for (std::size_t i=0;i<numberAngles;++i)
        {
            std::string name;
            double value;
            is >> name >> value;
            if (!_experiment->getDiffractometer()->getSample()->getGonio()->hasPhysicalAxis(name))
                QMessageBox::critical(this, tr("NSXTool"),
                                      tr("Physical axes in video file do not match instrument definition"));
        }
        // Get base directory where images are stored
        QFileInfo info(filename.c_str());
        QDir dir=info.absoluteDir();
        _filepath=dir.absolutePath().toStdString();

        // Attempt to read
        getline(file,line);
        while(!file.eof())
        {
            line.erase(std::remove(line.begin(), line.end(), ':'), line.end());
            std::stringstream is(line);
            std::string name,jpgfile;
            double value;
            is >> name >> value >> jpgfile;
            _imageList.push_back(std::pair<double,std::string>(value,_filepath+"/"+jpgfile));
            getline(file,line);
        }
        file.close();
        initializeSlider(_imageList.size()-1);
        // Load front image of the movie
        emit loadImage(QString::fromStdString(_imageList[0].second));
    }
    qDebug() << "Absorption Correction file: " << QString::fromStdString(filename);
    qDebug() << "Found:" << _imageList.size() << " images";
    ui->button_calibrateDistance->setEnabled(true);
}


void AbsorptionDialog::initializeSlider(int i)
{
    ui->horizontalScrollBar->setEnabled(true);
    ui->horizontalScrollBar->setRange(0,i);
}

void AbsorptionDialog::on_button_openFile_pressed()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);

    QString fileName=dialog.getOpenFileName(this,"Select Video file","",tr("Video file (*.info)"));
    // No file selected, do nothing
    if (fileName.isEmpty())
        return;
    readInfoFile(fileName.toStdString());
}

void AbsorptionDialog::setupInitialButtons()
{
    ui->button_calibrateDistance->setDisabled(true);
    ui->button_pickCenter->setDisabled(true);
    ui->button_pickingPoints->setDisabled(true);
    ui->button_removingPoints->setDisabled(true);
    ui->button_triangulate->setDisabled(true);
}

