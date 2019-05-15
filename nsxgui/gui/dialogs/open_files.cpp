
#include "nsxgui/gui/dialogs/open_files.h"
#include "nsxgui/gui/dialogs/file_dialog.h"
#include "nsxgui/qcr/engine/logger.h"
#include <QDir>
#include <QFileInfo>
#include <QStringBuilder>

namespace {
QDir dataDir{QDir::homePath()};
const QString dataFormats{"HDF5 files (*.hdf *.hdf5);;All files (*.*)"};
} // namespace

void open::addData(QWidget* parent)
{
    QStringList fileNames
            = file_dialog::queryImportFileNames(parent, "load data", dataDir, dataFormats);
    if (fileNames.isEmpty())
        return;
    for (QString file : fileNames)
        gLogger->log("data loaded: " + file);
    QFileInfo info{fileNames.at(0)};
    dataDir = info.absolutePath();
}

void open::addRaw(QWidget* parent)
{
    QString fileName = file_dialog::queryImportFileName(parent, "load raw", dataDir, dataFormats);
    if (fileName.isEmpty())
        return;
    gLogger->log("raw loaded: " + fileName);
    QFileInfo info{fileName};
    dataDir = info.absolutePath();
}
