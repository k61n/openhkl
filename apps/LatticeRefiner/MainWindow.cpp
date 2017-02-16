#include <fstream>
#include <sstream>

#include "ui_MainWindow.h"
#include "MainWindow.h"

#include <QDebug>
#include <QtAlgorithms>
#include <QFileDialog>
#include <QString>
#include <QStringList>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QContextMenuEvent>

#include <nsxlib/instrument/Detector.h>
#include <nsxlib/instrument/DiffractometerStore.h>
#include <nsxlib/instrument/Gonio.h>
#include <nsxlib/utils/LatticeConstraintParser.h>
#include <nsxlib/crystal/LatticeMinimizer.h>
#include <nsxlib/crystal/LatticeSolution.h>
#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/instrument/Source.h>
#include <nsxlib/utils/Units.h>

namespace qi = boost::spirit::qi;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),_peaks(),
    _minimizer(),
    _diffractometer(nullptr)
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

    ui->tableWidget_peaks->verticalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction* select=new QAction("Select/Unselect",ui->tableWidget_peaks->verticalHeader());
    ui->tableWidget_peaks->verticalHeader()->addAction(select);
    connect(select,SIGNAL(triggered()),this,SLOT(selectPeaks()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectPeaks()
{
    auto list=ui->tableWidget_peaks->selectionModel()->selectedRows();

    QColor c1("white");
    QColor c2(255,100,100);
    for (const auto& l: list)
    {
        bool selected=_peaks[l.row()].second;
        _peaks[l.row()].second=!selected;

        for (int i=0;i<ui->tableWidget_peaks->columnCount();++i)
            ui->tableWidget_peaks->item(l.row(),i)->setBackgroundColor(selected ? c2 : c1);
    }
    ui->tableWidget_peaks->clearSelection();
}

void MainWindow::on_actionOpen_reflections_triggered()
{

    ui->tableWidget_peaks->clear();

    QString filename=QFileDialog::getOpenFileName(this);

    std::vector<std::string> dnames;
    auto detector=_diffractometer->getDetector();
    auto dgonio = detector->getGonio();
    if (dgonio)
        dnames=dgonio->getPhysicalAxesNames();

    std::vector<std::string> snames;
    auto sample=_diffractometer->getSample();
    auto sgonio = sample->getGonio();
    if (sgonio)
        snames=sgonio->getPhysicalAxesNames();

    bool isBidim = detector->getNPixels()>1;

    int nBaseCols = 5;

    int nAngles = dnames.size()+snames.size();
    ui->tableWidget_peaks->setColumnCount(nBaseCols+nAngles);

    QStringList header;
    header.push_back("h");
    header.push_back("k");
    header.push_back("l");

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

            double px,py;
            if (isBidim)
                is >> px >> py;
            else
            {
                px=0.5;
                py=0.5;
            }
            ui->tableWidget_peaks->setItem(row,3,new QTableWidgetItem(QString::number(px)));
            ui->tableWidget_peaks->setItem(row,4,new QTableWidgetItem(QString::number(py)));

            int col=nBaseCols;
            std::vector<double> angles(nAngles);
            for (int i=0;i<nAngles;++i)
            {
                is >> angles[i];
                ui->tableWidget_peaks->setItem(row,col++,new QTableWidgetItem(QString::number(angles[i])));
            }

            std::vector<double> dangles(dnames.size()),sangles(snames.size());
            std::transform(angles.begin(),angles.begin()+dnames.size(),dangles.begin(),[&](double value)->double {return value*SX::Units::deg;});
            std::transform(angles.begin()+dnames.size(),angles.end(),sangles.begin(),[](double value)->double {return value*SX::Units::deg;});

            // Create a peak
            SX::Crystal::Peak3D peak;
//            peak.setMillerIndices(h,k,l);
            peak.setDetectorEvent(SX::Instrument::DetectorEvent(*_diffractometer->getDetector(), px,py,dangles));
            peak.setSampleState(std::shared_ptr<SX::Instrument::ComponentState>(new SX::Instrument::ComponentState(_diffractometer->getSample()->createState(sangles))));
            peak.setSource(_diffractometer->getSource());

            _peaks.push_back(std::pair<SX::Crystal::Peak3D,bool>(peak,true));

            row++;
        }
    }
}

void MainWindow::on_pushButton_refine_clicked()
{
    double val;

    val=ui->doubleSpinBox_a->value();
    _minimizer.setStartingValue(0,val,!ui->checkBox_a->isChecked());

    val=ui->doubleSpinBox_b->value();
    _minimizer.setStartingValue(1,val,!ui->checkBox_b->isChecked());

    val=ui->doubleSpinBox_c->value();
    _minimizer.setStartingValue(2,val,!ui->checkBox_c->isChecked());

    val=ui->doubleSpinBox_alpha->value();
    _minimizer.setStartingValue(3,val*SX::Units::deg,!ui->checkBox_alpha->isChecked());

    val=ui->doubleSpinBox_beta->value();
    _minimizer.setStartingValue(4,val*SX::Units::deg,!ui->checkBox_beta->isChecked());

    val=ui->doubleSpinBox_gamma->value();
    _minimizer.setStartingValue(5,val*SX::Units::deg,!ui->checkBox_gamma->isChecked());

    val=ui->doubleSpinBox_wavelength->value();
    _minimizer.setStartingValue(9,val,!ui->checkBox_wavelength->isChecked());

    // Parse the constraints string

    SX::Utils::LatticeConstraintParser<std::string::iterator> constraintsParser;

    SX::Utils::constraints_set constraints;
    std::string s=ui->lineEdit_constraints->text().toStdString();
    if (qi::phrase_parse(s.begin(),s.end(),constraintsParser,qi::blank,constraints))
    {
        for (auto c : constraints)
        {
            unsigned int lhs=std::get<0>(c);
            unsigned int rhs=std::get<1>(c);
            double factor=std::get<2>(c);
            _minimizer.setConstraint(lhs,rhs,factor);
        }
    }

    for (const auto& p : _peaks)
        if (p.second)
            _minimizer.addPeak(p.first);

    _minimizer.run(1000);

    SX::Crystal::LatticeSolution sol=_minimizer.getSolution();
    std::stringstream out;
    out<<sol;
    ui->textEdit_results->setText(QString::fromStdString(out.str()));
}

void MainWindow::on_comboBox_diffractometer_currentIndexChanged(const QString& diffractometerName)
{
    SX::Instrument::DiffractometerStore* ds = SX::Instrument::DiffractometerStore::Instance();

    _diffractometer=std::shared_ptr<SX::Instrument::Diffractometer>(ds->buildDiffractomer(diffractometerName.toStdString()));

    _minimizer.setDetector(_diffractometer->getDetector());
    _minimizer.setSample(_diffractometer->getSample());
    _minimizer.setSource(_diffractometer->getSource());
}

