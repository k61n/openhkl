
#include "nsxgui/gui/dialogs/file_dialog.h"
#include "nsxgui/qcr/base/debug.h"
#include "nsxgui/qcr/base/string_ops.h"
#include "nsxgui/qcr/engine/console.h"
#include "nsxgui/qcr/widgets/modal_dialogs.h"
#include <QFileSystemModel>
#include <QMessageBox>
#include <QSortFilterProxyModel>

namespace {

//  ***********************************************************************************************
//! @class OpenFileProxyModel for local use in queryImportFileNames.

class OpenFileProxyModel : public QSortFilterProxyModel {
public:
    int columnCount(const QModelIndex&) const { return 2; }
    QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const;
    QVariant data(const QModelIndex&, int = Qt::DisplayRole) const;
private:
    mutable QHash<QString, QString> memInfo;
};

QVariant OpenFileProxyModel::headerData(int section, Qt::Orientation ori, int role) const
{
    if (1 == section && Qt::Horizontal == ori && role == Qt::DisplayRole)
        return "Comment";
    return QSortFilterProxyModel::headerData(section, ori, role);
}

QVariant OpenFileProxyModel::data(const QModelIndex& idx, int role) const
{
    if (idx.isValid() && 1 == idx.column()) {
        if (role == Qt::DisplayRole) {
            QFileSystemModel* fileModel = qobject_cast<QFileSystemModel*>(sourceModel());
            QModelIndex ix0 =
                    fileModel->index(mapToSource(idx).row(), 0, mapToSource(idx.parent()));
            QFileInfo info(fileModel->rootDirectory().filePath(fileModel->fileName(ix0)));
            if (info.isFile()) {
                const QString& path = info.absoluteFilePath();
                auto it = memInfo.find(path);
                if (it != memInfo.end())
                    return *it;
            }
        }
        return QVariant();
    }
    return QSortFilterProxyModel::data(idx, role);
}

//  ***********************************************************************************************
//! @class FileDialog for local use in NSX file dialogs.

class FileDialog : public QcrFileDialog {
public:
    FileDialog(QWidget*, const QString&, QDir&, const QString& filter = "");
    QStringList getFiles();
    QString getFile();
private:
    QDir& dir_;
};

FileDialog::FileDialog(QWidget* parent, const QString& caption, QDir& dir, const QString& filter)
    : QcrFileDialog{parent, caption, dir.absolutePath(), filter}
    , dir_{dir}
{
    setOption(QFileDialog::DontUseNativeDialog);
    setViewMode(QFileDialog::Detail);
    setConfirmOverwrite(false);
}

QStringList FileDialog::getFiles()
{
    QStringList ret = selectedFiles();
    if (!ret.isEmpty())
        dir_ = QFileInfo(ret.at(0)).absolutePath();
    return ret;
}

QString FileDialog::getFile()
{
    QStringList files = getFiles();
    if (files.isEmpty())
        return "";
    return files.first();
}

} // namespace

//  ***********************************************************************************************

namespace file_dialog {

QStringList queryImportFileNames(
        QWidget* parent, const QString& caption, QDir& dir, const QString& filter, bool plural)
{
    FileDialog dlg{parent, caption, dir, filter};
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setReadOnly(true);
    dlg.setProxyModel(new OpenFileProxyModel);
    dlg.setFileMode(plural ? QFileDialog::ExistingFiles : QFileDialog::ExistingFile);
    if (!dlg.exec())
        return {};
    return dlg.getFiles();
}

QString queryImportFileName(
        QWidget* parent, const QString& caption, QDir& dir, const QString& filter)
{
    QStringList fileNames = queryImportFileNames(parent, caption, dir, filter, false);
    if (fileNames.isEmpty())
        return "";
    return fileNames.first();
}

QString queryExportFileName(
        QWidget* parent, const QString& caption, QDir& dir, const QString& filter)
{
    FileDialog dlg{parent, caption, dir, filter};
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    if (!dlg.exec())
        return "";
    return dlg.getFile();
}

QString queryDirectory(QWidget* parent, const QString& caption, const QString& dirname)
{
    QDir dir(dirname);
    FileDialog dlg{parent, caption, dir};
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    if (!dlg.exec())
        return "";
    return dlg.getFile();
}

} // namespace file_dialog

