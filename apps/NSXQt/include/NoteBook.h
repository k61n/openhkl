#ifndef NOTEBOOK_H
#define NOTEBOOK_H

#include <sstream>
#include <string>
#include <unordered_map>

#include <QTextEdit>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QImageReader>

class NoteBook : public QTextEdit
{
    Q_OBJECT
public:
    NoteBook(QWidget* parent=0);
    virtual ~NoteBook();

    bool canInsertFromMimeData(const QMimeData* source) const;
    void insertFromMimeData(const QMimeData* source);

private:

    void dropImage(const QUrl& url, const QImage& image);
    void dropTextFile(const QUrl& url);

public slots:
    void customMenuRequested(QPoint pos);
    void write2pdf();

};
#endif // NOTEBOOK_H
