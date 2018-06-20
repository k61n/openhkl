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

DialogHDF5Converter::DialogHDF5Converter(const nsx::DataList& numors, QWidget *parent)
: QDialog(parent),
  ui(new Ui::DialogHDF5Converter),
  _numors(numors)
{
    ui->setupUi(this);

    ui->outputDirectory->setText(QDir::currentPath());

    connect(ui->browseOutputDirectory,SIGNAL(clicked()),this,SLOT(browseOutputDirectory()));
    connect(ui->convert,SIGNAL(clicked()),this,SLOT(convert()));
}

DialogHDF5Converter::~DialogHDF5Converter()
{
    delete ui;
}

void DialogHDF5Converter::convert()
{
    if (_numors.size() == 0) {
        QMessageBox::warning(this,"Output directory","No numors selected for conversion");
        return;
    }

    QString outputDirectory = ui->outputDirectory->text();
    if (outputDirectory.isEmpty())
    {
        QMessageBox::warning(this,"Output directory","Please enter an output directory.");
        return;
    }

    ui->progressBar_conversion->setMaximum(_numors.size());
    ui->progressBar_conversion->setValue(0);

    int comp(0);
    for (auto numor : _numors) {

        auto&& numor_filename = numor->filename();

        QFileInfo fileinfo(QString::fromStdString(numor_filename));
        QString basename = fileinfo.baseName();

        auto hdf5_filename = QDir(outputDirectory).filePath(basename+".h5").toStdString();

        if (hdf5_filename.compare(numor_filename)==0) {
            return;
        }

        try {
            numor->saveHDF5(hdf5_filename);
        } catch(...) {
            nsx::error() << "The filename " << hdf5_filename << " could not be saved. Maybe a permission problem.";
        }
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
