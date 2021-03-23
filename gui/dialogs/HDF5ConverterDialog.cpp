
#include "gui/dialogs/HDF5ConverterDialog.h"

#include "core/data/DataSet.h"
#include "gui/MainWin.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

HDF5ConverterDialog::HDF5ConverterDialog()
{
    QVBoxLayout* wholeLayout = new QVBoxLayout(this);
    QHBoxLayout* firstLine = new QHBoxLayout;
    directory = new QLineEdit(QDir::homePath());
    directory->setReadOnly(true);
    browse = new QPushButton("...");
    firstLine->addWidget(directory);
    firstLine->addWidget(browse);
    QHBoxLayout* secondLine = new QHBoxLayout;
    filename = new QLineEdit("");
    secondLine->addWidget(filename);
    secondLine->addWidget(new QLabel(".h5"));
    progress = new QProgressBar(this);
    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    wholeLayout->addLayout(firstLine);
    wholeLayout->addLayout(secondLine);
    wholeLayout->addWidget(progress);
    wholeLayout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &HDF5ConverterDialog::convert);
    connect(buttons, &QDialogButtonBox::rejected, this, &HDF5ConverterDialog::reject);
    connect(browse, &QPushButton::clicked, this, &HDF5ConverterDialog::browseDirectory);
}

void HDF5ConverterDialog::browseDirectory()
{
    QString directoryPath =
        QFileDialog::getExistingDirectory(this, "Browse Directory", directory->text());
    directory->setText(directoryPath);
}

void HDF5ConverterDialog::convert()
{
    QStringList numors = gSession->currentProject()->getDataNames();
    if (numors.empty()) {
        // gLogger->log("[ERROR] No numors selected, conversion aborted");
        return;
    }
    QString outputDirectory = directory->text();
    if (outputDirectory.isEmpty()) {
        // gLogger->log("[ERROR] No output directory selected, conversion aborted");
        return;
    }
    progress->setMaximum(numors.size());
    progress->setValue(0);
    int comp = 0;
    for (const QString& numor : numors) {
        std::string numor_filename = numor.toStdString();
        std::string hdfFilename =
            QDir(outputDirectory).filePath(filename->text() + ".h5").toStdString();
        if (hdfFilename.compare(numor_filename) == 0) {
            // gLogger->log("[ERROR] numor filename and target filename equal, conversion aborted");
            return;
        }

        try {
            gSession->currentProject()->getData(comp)->saveHDF5(hdfFilename);
        } catch (...) {
            // gLogger->log(
            //     "[ERROR] The filename " + QString::fromStdString(hdfFilename)
            //     + " could not be saved. May be a permission problem.");
        }
        progress->setValue(++comp);
    }

    accept();
}
