#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QString>
#include <QStringList>

#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/InstrumentTypes.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Path.h>
#include <nsxlib/Resources.h>

#include "DataItem.h"
#include "DialogHDF5Converter.h"
#include "ui_DialogHDF5Converter.h"

DialogHDF5Converter::DialogHDF5Converter(nsx::sptrExperiment experiment, QWidget *parent)
: QDialog(parent),
  ui(new Ui::DialogHDF5Converter),
  _experiment(experiment)
{
    ui->setupUi(this);

    ui->outputDirectory->setText(QDir::currentPath());

    connect(ui->browseInputNumors,SIGNAL(clicked()),this,SLOT(browseInputNumors()));
    connect(ui->browseOutputDirectory,SIGNAL(clicked()),this,SLOT(browseOutputDirectory()));
    connect(ui->convert,SIGNAL(clicked()),this,SLOT(convert()));
}

DialogHDF5Converter::~DialogHDF5Converter()
{
    delete ui;
}

void DialogHDF5Converter::browseInputNumors()
{
    QStringList numors = QFileDialog::getOpenFileNames(this, tr("Open ILL ASCII numors"), ".");

    if (numors.isEmpty()) {
        return;
    }

    for (size_t i = 0; i < ui->numors->count(); ++i) {
        numors.append(ui->numors->item(i)->text());
    }

    numors.removeDuplicates();

    ui->numors->clear();
    ui->numors->addItems(numors);
}

void DialogHDF5Converter::convert()
{
    if (ui->numors->count() == 0) {
        QMessageBox::warning(this,"Output directory","No numors selected for conversion");
        return;
    }

    QString outputDirectory = ui->outputDirectory->text();
    if (outputDirectory.isEmpty())
    {
        QMessageBox::warning(this,"Output directory","Please enter an output directory.");
        return;
    }

    ui->progressBar_conversion->setMaximum(ui->numors->count());
    ui->progressBar_conversion->setValue(0);

    auto diffractometer = nsx::Diffractometer::build(_experiment->diffractometer()->name());

    nsx::DataReaderFactory dataFactory;

    int comp(0);
    while(ui->numors->count()) {

        auto item = ui->numors->item(0);
        auto numor = item->text().toStdString();

        QFileInfo fileinfo(QString::fromStdString(numor));
        QString basename = fileinfo.baseName();
        auto extension = fileinfo.suffix().toStdString();
        auto outputFilename = QDir(outputDirectory).filePath(basename+".h5").toStdString();

        std::shared_ptr<nsx::DataSet> data;
        try {
            data = dataFactory.create(extension,numor,diffractometer);
        } catch(std::exception& e) {
            nsx::error() << "opening file " << numor << e.what();
            ui->progressBar_conversion->setValue(++comp);
            if (data) {
                data.reset();
            }
            delete item;
            continue;
        }

        try {
            data->saveHDF5(outputFilename);
        } catch(...) {
            nsx::error() << "The filename " << outputFilename << " could not be saved. Maybe a permission problem.";
            ui->progressBar_conversion->setValue(++comp);
            data.reset();
            delete item;
            continue;
        }
        data.reset();
        delete item;
        ui->numors->repaint();
        ui->progressBar_conversion->setValue(++comp);
    }
}

void DialogHDF5Converter::browseOutputDirectory()
{
    QString outputDirectory = QFileDialog::getExistingDirectory (this, "Enter output directory", ".", QFileDialog::ShowDirsOnly);
    if (!outputDirectory.isEmpty()) {
        ui->outputDirectory->setText(outputDirectory);
    }
}
