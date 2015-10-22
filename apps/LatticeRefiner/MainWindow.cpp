#include <fstream>
#include <sstream>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DiffractometerStore.h"
#include "Diffractometer.h"
#include "Detector.h"
#include "Sample.h"
#include "Gonio.h"

#include <QtAlgorithms>
#include <QFileDialog>
#include <QString>
#include <QStringList>
#include <QHeaderView>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _peaks()
{
    ui->setupUi(this);

    ui->checkBox_wavelength->setText(QString((QChar) 0x03BB));
    ui->checkBox_alpha->setText(QString((QChar) 0x03B1));
    ui->checkBox_beta->setText(QString((QChar) 0x03B2));
    ui->checkBox_gamma->setText(QString((QChar) 0x03B3));

    // The instrument names will be inserted alphabetically
    ui->comboBox_diffractometer->setInsertPolicy(QComboBox::InsertAlphabetically);

    // Add the available instruments to the combo box
    SX::Instrument::DiffractometerStore* ds = SX::Instrument::DiffractometerStore::Instance();
    for (const auto& diffractometer : ds->getDiffractometersList())
        ui->comboBox_diffractometer->addItem(QString::fromStdString(diffractometer));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_reflections_triggered()
{

    ui->tableWidget_peaks->clear();

    QString filename=QFileDialog::getOpenFileName(this);

    SX::Instrument::DiffractometerStore* ds = SX::Instrument::DiffractometerStore::Instance();
    std::string diffractometerName=ui->comboBox_diffractometer->currentText().toStdString();

    std::shared_ptr<SX::Instrument::Diffractometer> diffractometer=std::shared_ptr<SX::Instrument::Diffractometer>(ds->buildDiffractomer(diffractometerName));

    auto detector=diffractometer->getDetector();
    auto dnames=detector->getGonio()->getPhysicalAxesNames();
    auto sample=diffractometer->getSample();
    auto snames=sample->getGonio()->getPhysicalAxesNames();

    bool isBidim = detector->getNPixels()>1;
    int nPixelColumns = isBidim ? 2 : 0;

    int nAngles = dnames.size()+snames.size();
    ui->tableWidget_peaks->setColumnCount(3+nPixelColumns+nAngles);

    QStringList header;
    header.push_back(QString("h"));
    header.push_back("k");
    header.push_back("l");

    if (isBidim)
    {
        header.push_back("px");
        header.push_back("py");
    }

    for (const auto& v : dnames)
        header.push_back(QString::fromStdString(v));

    for (const auto& v : snames)
        header.push_back(QString::fromStdString(v));

    ui->tableWidget_peaks->setHorizontalHeaderLabels(header);
    ui->tableWidget_peaks->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    std::fstream inputFile;

    inputFile.open(filename.toStdString(),std::ios::in);

    if (inputFile.is_open())
    {
        int row=0;
        std::string line;

        while (std::getline(inputFile,line))
        {
            double h,k,l;

            std::istringstream is(line);
            is >> h >> k >> l;

            ui->tableWidget_peaks->insertRow(row);
            ui->tableWidget_peaks->setItem(row,0,new QTableWidgetItem(QString::number(h)));
            ui->tableWidget_peaks->setItem(row,1,new QTableWidgetItem(QString::number(k)));
            ui->tableWidget_peaks->setItem(row,2,new QTableWidgetItem(QString::number(l)));

            if (isBidim)
            {
                double px,py;
                is >> px >> py;
                ui->tableWidget_peaks->setItem(row,3,new QTableWidgetItem(QString::number(px)));
                ui->tableWidget_peaks->setItem(row,4,new QTableWidgetItem(QString::number(py)));
            }

            std::vector<double> angles(nAngles);
            for (int i=0;i<nAngles;++i)
            {
                is >> angles[i];
                ui->tableWidget_peaks->setItem(row,3+nPixelColumns+i,new QTableWidgetItem(QString::number(angles[i])));
            }


//            // Create a peak
//            SX::Crystal::Peak3D peak;
//            // Create the detector event matching that peak (the px and py are given in mm in the RAFUB input file)
//            peak.setDetectorEvent(new DetectorEvent(D9->createDetectorEvent(px/2,py/2,{gamma*deg})));

//            // set the miller indices corresponding to the peak
//            peak.setMillerIndices(h,k,l);
//            // Set the wavelength
//            peak.setSource(source);

//            // Create a sample state
//            peak.setSampleState(new ComponentState(sample->createState({omega*deg,chi*deg,phi*deg})));
//            _peaks.push_back(peak);



            row++;
        }
    }





}
