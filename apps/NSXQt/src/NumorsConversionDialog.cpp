#include <memory>

#include <QDirModel>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "DataReaderFactory.h"
#include "Diffractometer.h"
#include "DiffractometerFactory.h"
#include "IData.h"

#include "include/NumorsConversionDialog.h"
#include "ui_NumorsConversionDialog.h"

NumorsConversionDialog::NumorsConversionDialog(QWidget *parent)
: QDialog(parent),
  ui(new Ui::NumorsConversionDialog)
{
    ui->setupUi(this);

    // The instrument names will be inserted alphabetically
    ui->comboBox_diffractometers->setInsertPolicy(QComboBox::InsertAlphabetically);

    // Add the available instruments to the combo box
    SX::Instrument::DiffractometerFactory* diffractomersFactory = SX::Instrument::DiffractometerFactory::Instance();
    for (const auto& diffractometer : diffractomersFactory->list())
        ui->comboBox_diffractometers->addItem(QString::fromStdString(diffractometer));

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
        QMessageBox::warning(this,"Output directorty",QString::fromStdString("Please enter an output directory."));
        return;
    }

    SX::Data::DataReaderFactory* dataFactory=SX::Data::DataReaderFactory::Instance();
    SX::Instrument::DiffractometerFactory* diffractometersFactory=SX::Instrument::DiffractometerFactory::Instance();
    std::string diffractometerName=ui->comboBox_diffractometers->currentText().toStdString();
    std::shared_ptr<SX::Instrument::Diffractometer> diffractometer=std::shared_ptr<SX::Instrument::Diffractometer>(diffractometersFactory->create(diffractometerName,"instrument")->clone());

    auto model = dynamic_cast<QDirModel*>(ui->treeView_inputFiles->model());
    QModelIndexList indexes = ui->treeView_inputFiles->selectionModel()->selectedIndexes();

    ui->progressBar_conversion->setMaximum(indexes.size());
    ui->progressBar_conversion->setValue(0);

    int comp(0);
    for (auto& idx : indexes)
    {
        QFileInfo fileInfo = model->fileInfo(idx);
        if (fileInfo.isFile())
        {
            std::string filename=fileInfo.absoluteFilePath().toStdString();
            std::string extension=fileInfo.completeSuffix().toStdString();
            SX::Data::IData* data = dataFactory->create(extension,filename,diffractometer);
            data->readInMemory();
            QString basename=fileInfo.baseName();
            QString outputFilename = QDir(ui->lineEdit_outputDirectory->text()).filePath(basename+".h5");
            data->saveHDF5(outputFilename.toStdString());
            delete data;
        }
        ui->progressBar_conversion->setValue(++comp);
    }
}

void NumorsConversionDialog::on_pushButton_browse_clicked()
{
    QString outputDirectory=QFileDialog::getExistingDirectory (this, "Enter output directory", QDir::homePath());
    if (!outputDirectory.isEmpty())
        ui->lineEdit_outputDirectory->setText(outputDirectory);
}
