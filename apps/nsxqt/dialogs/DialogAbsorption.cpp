#include <algorithm>
#include <fstream>
#include <sstream>

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/Logger.h>
#include <nsxlib/RotAxis.h>
#include <nsxlib/Sample.h>

#include "CrystalScene.h"
#include "DialogAbsorption.h"

#include "ui_DialogAbsorption.h"

DialogAbsorption::DialogAbsorption(nsx::sptrExperiment experiment, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbsorption),
    _experiment(experiment),
    _cscene(new CrystalScene(&experiment->diffractometer()->sample()->shape()))
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

DialogAbsorption::~DialogAbsorption()
{
    delete ui;
}

const std::string& DialogAbsorption::getMovieFilename() const
{
    return _filepath;
}

void DialogAbsorption::readInfoFile(const std::string &filename)
{
    // Clear alll images.
    _imageList.clear();
    _imageList.shrink_to_fit();

    std::ifstream file;
    file.open(filename.c_str(),std::ios::in);

    if (file.is_open()) {
        // First read instrument name and validate with diffractometer name
        std::string _instrumentName, date;
        file >> _instrumentName >> date;
        std::string diffType=_experiment->diffractometer()->name();
        if (_instrumentName.compare(diffType)!=0) {
            QMessageBox::critical(this, tr("NSXTool"), tr("Instrument name in video file does not match the diffractometer name"));
        }


        std::string line;
        // Skip one line (comment)
        getline(file,line);
        getline(file,line);
        // Read line with goniometer angles
        getline(file,line);
        // Cout number of axes, validate with goniometer definition
        auto sample = _experiment->diffractometer()->sample();
        const auto& sample_gonio = sample->gonio();
        std::size_t numberAngles = std::count(line.begin(),line.end(),':');
        if (numberAngles == sample_gonio.nAxes()) {
            QMessageBox::critical(this, tr("NSXTool"), tr("Number of goniometer axes in video file does not match instrument definition"));
        }

        // Remove all occurences of ':' before reading
        line.erase(std::remove(line.begin(), line.end(), ':'), line.end());
        std::stringstream is(line);
        for (std::size_t i = 0; i < numberAngles; ++i) {
            std::string name;
            double value;
            is >> name >> value;
            const auto &axis = sample_gonio.axis(i);
            if (axis.name().compare(name) != 0) {
                QMessageBox::critical(this, tr("NSXTool"), tr("Mismatch between axis names in video file and in instrument definition"));
            }
        }

        // Get base directory where images are stored
        QFileInfo info(filename.c_str());
        QDir dir=info.absoluteDir();
        _filepath=dir.absolutePath().toStdString();
        //
        getline(file,line);
        if (line.find("camera:")!=std::string::npos) {
            getline(file,line);
        }
        while(!file.eof()) {
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
    nsx::info() << "absorption correction file: " << filename;
    nsx::info() << "found:" << _imageList.size() << " images";
    ui->button_calibrateDistance->setEnabled(true);
}


void DialogAbsorption::initializeSlider(int i)
{
    ui->horizontalScrollBar->setEnabled(true);
    ui->horizontalScrollBar->setRange(0,i);
}

void DialogAbsorption::on_button_openFile_pressed()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);

    QString fileName=dialog.getOpenFileName(this,"Select Video file","",tr("Video file (*.info)"));
    // No file selected, do nothing
    if (fileName.isEmpty()) {
        return;
    }
    readInfoFile(fileName.toStdString());
}

void DialogAbsorption::setupInitialButtons()
{
    ui->button_calibrateDistance->setDisabled(true);
    ui->button_pickCenter->setDisabled(true);
    ui->button_pickingPoints->setDisabled(true);
    ui->button_removingPoints->setDisabled(true);
    ui->button_triangulate->setDisabled(true);
}

