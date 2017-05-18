#ifndef NSXQT_NOTEBOOK_H
#define NSXQT_NOTEBOOK_H

#include <sstream>
#include <string>
#include <unordered_map>

#include <QTextEdit>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QImageReader>

/*
 * A QtextEdit inherited class in which image and text can be dragged and dropped in.
 * */
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
#endif // NSXQT_NOTEBOOK_H
