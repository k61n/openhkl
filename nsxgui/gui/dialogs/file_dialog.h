
#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <QDir>
#include <QFile>

namespace file_dialog {

QStringList queryImportFileNames(QWidget*, const QString&, QDir&, const QString&, bool plural=true);
QString queryImportFileName(QWidget*, const QString&, QDir&, const QString&);
QString queryExportFileName(QWidget*, const QString&, QDir&, const QString&);
QString queryDirectory(QWidget*, const QString&, const QString&);

} // namespace file_dialog

#endif // FILE_DIALOG_H
