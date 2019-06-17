
#include "gui/dialogs/HDF5ConverterDialog.h"

#include "gui/MainWin.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

HDF5ConverterDialog::HDF5ConverterDialog()
    : QcrModalDialog("adhoc_convertHdf", gGui, "Convert to HDF5")
{
    QVBoxLayout* wholeLayout = new QVBoxLayout(this);
    QHBoxLayout* firstLine = new QHBoxLayout;
    directory = new QcrLineEdit("adhoc_directory", QDir::homePath());
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
}

void HDF5ConverterDialog::browseDirectory()
{

}

void HDF5ConverterDialog::convert()
{

}
