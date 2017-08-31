#include <QDir>
#include <QDirModel>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QModelIndex>
#include <QModelIndexList>
#include <QString>
#include <QStringList>

#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/instrument/InstrumentTypes.h>
#include <nsxlib/logger/Logger.h>
#include <nsxlib/utils/Path.h>

#include "NumorsConversionDialog.h"
#include "ui_NumorsConversionDialog.h"

NumorsConversionDialog::NumorsConversionDialog(QWidget *parent)
: QDialog(parent),
  ui(new Ui::NumorsConversionDialog)
{
    ui->setupUi(this);

    // The instrument names will be inserted alphabetically
    ui->comboBox_diffractometers->setInsertPolicy(QComboBox::InsertAlphabetically);

    QDir diffractometersDirectory(QString::fromStdString(nsx::applicationDataPath()));
    diffractometersDirectory.cd("instruments");

    QStringList diffractometerFiles = diffractometersDirectory.entryList({"*.yml"}, QDir::Files, QDir::Name);

    // Add the available instruments to the combo box
    for (auto&& diffractometer : diffractometerFiles) {
        ui->comboBox_diffractometers->addItem(QFileInfo(diffractometer).baseName());
    }

    QDirModel* model=new QDirModel();
    ui->treeView_inputFiles->setModel(model);

    ui->treeView_inputFiles->setRootIndex(model->index(QDir::homePath()));
    ui->treeView_inputFiles->setColumnHidden( 1, true );
    ui->treeView_inputFiles->setColumnHidden( 2, true );
    ui->treeView_inputFiles->setColumnHidden( 3, true );

    ui->treeView_inputFiles->setWindowTitle(QObject::tr("Dir View:")+QDir::homePath());
}

NumorsConversionDialog::~NumorsConversionDialog()
{
    delete ui;
}

void NumorsConversionDialog::on_pushButton_convert_clicked()
{

    QString outputDirectory=ui->lineEdit_outputDirectory->text();
    if (outputDirectory.isEmpty())
    {
        QMessageBox::warning(this,"Output directory",QString::fromStdString("Please enter an output directory."));
        return;
    }

    nsx::DataReaderFactory dataFactory;
    std::string diffractometerName=ui->comboBox_diffractometers->currentText().toStdString();
    auto diffractometer = nsx::Diffractometer::build(diffractometerName);

    auto model = dynamic_cast<QDirModel*>(ui->treeView_inputFiles->model());
    QModelIndexList indexes = ui->treeView_inputFiles->selectionModel()->selectedIndexes();

    ui->progressBar_conversion->setMaximum(indexes.size());
    ui->progressBar_conversion->setValue(0);

    int comp(0);
    for (auto& idx : indexes) {
        QFileInfo fileInfo = model->fileInfo(idx);
        int row = -1;
        if (idx.row()!=row && idx.column()==0) {
            row = idx.row();
            std::string filename=fileInfo.absoluteFilePath().toStdString();
            std::string extension=fileInfo.completeSuffix().toStdString();
            std::shared_ptr<nsx::DataSet> data;
            try {
                data = dataFactory.create(extension,filename,diffractometer);
            }
            catch(std::exception& e) {
                nsx::error() << "opening file " << filename.c_str() << e.what();
                ui->progressBar_conversion->setValue(++comp);
                if (data) {
                    data.reset();
                }
                continue;
            }
            // todo: implement progress handler here
            QString basename=fileInfo.baseName();
            QString outputFilename = QDir(ui->lineEdit_outputDirectory->text()).filePath(basename+".h5");

            try {
                data->saveHDF5(outputFilename.toStdString());
            } catch(...) {
                nsx::error() << "The filename " << filename.c_str() << " could not be saved. Maybe a permission problem.";
                ui->progressBar_conversion->setValue(++comp);
                data.reset();
                continue;
            }
            data.reset();
        }
        ui->progressBar_conversion->setValue(++comp);
    }
}

void NumorsConversionDialog::on_pushButton_browse_clicked()
{
    QString outputDirectory=QFileDialog::getExistingDirectory (this, "Enter output directory", QDir::homePath());
    if (!outputDirectory.isEmpty()) {
        ui->lineEdit_outputDirectory->setText(outputDirectory);
    }
}
