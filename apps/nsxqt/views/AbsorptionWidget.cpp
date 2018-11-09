#include <algorithm>
#include <fstream>
#include <sstream>

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPixmap>

#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/RotAxis.h>
#include <nsxlib/Sample.h>

#include "AbsorptionWidget.h"
#include "ui_AbsorptionWidget.h"

AbsorptionWidget::AbsorptionWidget(nsx::Experiment* experiment,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AbsorptionWidget),
    _experiment(experiment)
{
    ui->setupUi(this);
    connect(ui->horizontalScrollBar,SIGNAL(valueChanged(int)),this,SLOT(loadImage(int)));
    ui->graphicsView->setScene(new QGraphicsScene());
}

AbsorptionWidget::~AbsorptionWidget()
{
    delete ui;
}


void AbsorptionWidget::on_pushButton_clicked()
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

void AbsorptionWidget::readInfoFile(const std::string &filename)
{
    std::ifstream file;
    file.open(filename.c_str(),std::ios::in);

    if (file.is_open()) {
        // First read instrument name and validate with diffractometer name
        std::string _instrumentName, date;
        file >> _instrumentName >> date;
        if (_instrumentName.compare(_experiment->diffractometer()->name())!=0) {
            QMessageBox::critical(this, tr("NSXTool"), tr("Unknown instrument name"));
        }

        std::string line;
        // Skip two line (comment)
        getline(file,line);
        getline(file,line);

        // Read line with goniometer angles
        getline(file,line);
        // Cout number of axes, validate with goniometer definition
        std::size_t numberAngles = std::count(line.begin(),line.end(),':');
        auto sample = _experiment->diffractometer()->sample();
        const auto &sample_gonio = sample->gonio();
        if (numberAngles == sample_gonio.nAxes()) {
            QMessageBox::critical(this, tr("NSXTool"), tr("Number of goniometer axes in video file do not match instrument definition"));
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
                QMessageBox::critical(this, tr("NSXTool"), tr("Physical axes in video file do not match instrument definition"));
            }
        }
        // Get base directory where images are stored
        QFileInfo info(filename.c_str());
        QDir dir=info.absoluteDir();
        _filepath=dir.absolutePath().toStdString();

        // Attempt to read
        getline(file,line);
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
    }
}

void AbsorptionWidget::loadImage(unsigned int i)
{
    if (i>=_imageList.size()) {
        return;
    }
    QPixmap pix;
    QString file=QString::fromStdString(_imageList[i].second);
    pix.load(QString::fromStdString(_imageList[i].second));

    // unused variable
    //QGraphicsPixmapItem* item=
    ui->graphicsView->scene()->addPixmap(pix);
    update();
}

void AbsorptionWidget::initializeSlider(int i)
{
    ui->horizontalScrollBar->setEnabled(true);
    ui->horizontalScrollBar->setRange(0,i);
}
