
#include "gui/dialogs/HDF5ConverterDialog.h"

#include "gui/MainWin.h"
#include "core/experiment/DataSet.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QFileDialog>

HDF5ConverterDialog::HDF5ConverterDialog()
    : QDialog{}
{
    QVBoxLayout* wholeLayout = new QVBoxLayout(this);
    QHBoxLayout* firstLine = new QHBoxLayout;
    directory = new QcrLineEdit("adhoc_directory", QDir::homePath());
    directory->setReadOnly(true);
    QcrTrigger* trigger = new QcrTrigger("adhoc_browseDirectory", "browse directory");
    trigger->setTriggerHook([=](){ browseDirectory(); });
    browse = new QcrTextTriggerButton(trigger);
    firstLine->addWidget(directory);
    firstLine->addWidget(browse);
    QHBoxLayout* secondLine = new QHBoxLayout;
    filename = new QcrLineEdit("adhoc_hdfname", "");
    secondLine->addWidget(filename);
    secondLine->addWidget(new QLabel(".h5"));
    progress = new QProgressBar(this);
    buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal);
    wholeLayout->addLayout(firstLine);
    wholeLayout->addLayout(secondLine);
    wholeLayout->addWidget(progress);
    wholeLayout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &HDF5ConverterDialog::convert);
    connect(buttons, &QDialogButtonBox::rejected, this, &HDF5ConverterDialog::reject);
}

void HDF5ConverterDialog::browseDirectory()
{
    QString directoryPath = QFileDialog::getExistingDirectory(this,
                                                              "Browse Directory",
                                                              directory->getValue());
    directory->setCellValue(directoryPath);
}

void HDF5ConverterDialog::convert()
{
    nsx::DataList numors = gSession->selectedExperiment()->data()->allDataVector();
    if (numors.empty()) {
        gLogger->log("[ERROR] No numors selected, conversion aborted");
        return;
    }
    QString outputDirectory = directory->getValue();
    if (outputDirectory.isEmpty()) {
        gLogger->log("[ERROR] No output directory selected, conversion aborted");
        return;
    }
    progress->setMaximum(numors.size());
    progress->setValue(0);
    int comp(0);
    for (nsx::sptrDataSet numor : numors) {
        std::string numor_filename = numor->filename();
        std::string hdfFilename = QDir(outputDirectory)
                .filePath(filename->getValue() + ".h5")
                .toStdString();
        if (hdfFilename.compare(numor_filename) == 0) {
            gLogger->log("[ERROR] numor filename and target filename equal, conversion aborted");
            return;
        }

        try {
            numor->saveHDF5(hdfFilename);
        } catch (...) {
            gLogger->log("[ERROR] The filename " + QString::fromStdString(hdfFilename)
                         + " could not be saved. May be a permission problem.");
        }
        progress->setValue(++comp);
    }

    accept();
}
