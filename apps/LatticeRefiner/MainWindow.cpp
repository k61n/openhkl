#include <fstream>
#include <sstream>

#include "ui_MainWindow.h"
#include "MainWindow.h"

#include "Detector.h"
#include "Diffractometer.h"
#include "DiffractometerStore.h"
#include "Gonio.h"
#include "LatticeConstraintParser.h"
#include "LatticeMinimizer.h"
#include "Peak3D.h"
#include "Sample.h"
#include "Source.h"

#include <QDebug>
#include <QtAlgorithms>
#include <QFileDialog>
#include <QString>
#include <QStringList>
#include <QHeaderView>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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
//    int nPixelColumns = isBidim ? 2 : 0;

//    int nBaseCols = 3+nPixelColumns;
    int nBaseCols = 5;

    int nAngles = dnames.size()+snames.size();
    ui->tableWidget_peaks->setColumnCount(nBaseCols+nAngles+1);

    QStringList header;
    header.push_back(QString("h"));
    header.push_back("k");
    header.push_back("l");

//    if (isBidim)
//    {
//        header.push_back("px");
//        header.push_back("py");
//    }
    header.push_back("px");
    header.push_back("py");

    for (const auto& v : dnames)
        header.push_back(QString::fromStdString(v));

    for (const auto& v : snames)
        header.push_back(QString::fromStdString(v));

    header.push_back("select");

    ui->tableWidget_peaks->setHorizontalHeaderLabels(header);
    ui->tableWidget_peaks->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    if (!isBidim)
    {
        ui->tableWidget_peaks->hideColumn(3);
        ui->tableWidget_peaks->hideColumn(4);
    }

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
            else
            {
                ui->tableWidget_peaks->setItem(row,3,new QTableWidgetItem(QString("0.5")));
                ui->tableWidget_peaks->setItem(row,4,new QTableWidgetItem(QString("0.5")));
            }


            int col=nBaseCols;
            std::vector<double> angles(nAngles);
            for (int i=0;i<nAngles;++i)
            {
                is >> angles[i];
                ui->tableWidget_peaks->setItem(row,col++,new QTableWidgetItem(QString::number(angles[i])));
            }

            ui->tableWidget_peaks->setItem(row,col,new QTableWidgetItem(""));
            ui->tableWidget_peaks->item(row,col)->setCheckState(Qt::Checked);

            row++;
        }
    }
}

void MainWindow::on_pushButton_refine_clicked()
{
    SX::Instrument::DiffractometerStore* ds = SX::Instrument::DiffractometerStore::Instance();
    std::string diffractometerName=ui->comboBox_diffractometer->currentText().toStdString();

    std::shared_ptr<SX::Instrument::Diffractometer> diffractometer=std::shared_ptr<SX::Instrument::Diffractometer>(ds->buildDiffractomer(diffractometerName));

    SX::Crystal::LatticeMinimizer minimizer;
    minimizer.setDetector(diffractometer->getDetector());
    minimizer.setSample(diffractometer->getSample());
    minimizer.setSource(diffractometer->getSource());

    // Fetch the starting or fixed value for a,b,c,alpha,beta,gamma and lambda

    double val;

    val=ui->doubleSpinBox_a->value();
    if (ui->checkBox_a->isChecked())
        minimizer.setStartingValue(0,val);
    else
        minimizer.setLatticeFixedValue(0,val);

    val=ui->doubleSpinBox_b->value();
    if (ui->checkBox_b->isChecked())
        minimizer.setStartingValue(1,val);
    else
        minimizer.setLatticeFixedValue(1,val);

    val=ui->doubleSpinBox_c->value();
    if (ui->checkBox_c->isChecked())
        minimizer.setStartingValue(2,val);
    else
        minimizer.setLatticeFixedValue(2,val);

    val=ui->doubleSpinBox_alpha->value();
    if (ui->checkBox_alpha->isChecked())
        minimizer.setStartingValue(3,val);
    else
        minimizer.setLatticeFixedValue(3,val);

    val=ui->doubleSpinBox_beta->value();
    if (ui->checkBox_beta->isChecked())
        minimizer.setStartingValue(4,val);
    else
        minimizer.setLatticeFixedValue(4,val);

    val=ui->doubleSpinBox_gamma->value();
    if (ui->checkBox_gamma->isChecked())
        minimizer.setStartingValue(5,val);
    else
        minimizer.setLatticeFixedValue(5,val);

    val=ui->doubleSpinBox_wavelength->value();
    if (ui->checkBox_wavelength->isChecked())
        minimizer.setStartingValue(9,val);
    else
        minimizer.setLatticeFixedValue(9,val);

    // Parse the constraints string

    SX::Utils::LatticeConstraintParser<std::string::iterator> constraintsParser;

    SX::Utils::constraints_set constraints;
    std::string s=ui->lineEdit_constraints->text().toStdString();
    if (qi::phrase_parse(s.begin(),s.end(),constraintsParser,qi::blank,constraints))
    {
        for (auto c : constraints)
            minimizer.setLatticeConstraint(std::get<0>(c),std::get<1>(c));
    }

    // Set the peaks

    int lastColumn = ui->tableWidget_peaks->columnCount()-1;
    for (int row=0;row<ui->tableWidget_peaks->rowCount();++row)
    {

        if (ui->tableWidget_peaks->item(row,lastColumn)->checkState()==Qt::Unchecked)
            continue;

        // Create a peak
        SX::Crystal::Peak3D peak;

        double h,k,l;
        h=ui->tableWidget_peaks->item(row,0)->text().toDouble();
        k=ui->tableWidget_peaks->item(row,1)->text().toDouble();
        l=ui->tableWidget_peaks->item(row,2)->text().toDouble();
        peak.setMillerIndices(h,k,l);
        double px,py;
        px=ui->tableWidget_peaks->item(row,3)->text().toDouble();
        py=ui->tableWidget_peaks->item(row,4)->text().toDouble();
//        peak.setDetectorEvent(new DetectorEvent(D9->createDetectorEvent(px/2,py/2,{gamma*deg})));

        // Set the wavelength
        diffractometer->getSource()->setWavelength(ui->doubleSpinBox_wavelength->value());
        peak.setSource(diffractometer->getSource());

        // Create a sample state
//        peak.setSampleState(new ComponentState(sample->createState({omega*deg,chi*deg,phi*deg})));

        minimizer.addPeak(peak);
    }

}
