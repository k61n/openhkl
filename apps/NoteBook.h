#pragma once

#include <sstream>
#include <string>
#include <unordered_map>

#include <QFileInfo>
#include <QImageReader>
#include <QMimeData>
#include <QTextEdit>
#include <QUrl>

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

public slots:

    void printLogMessage(const std::string& message);

private:

    void dropImage(const QUrl& url, const QImage& image);
    void dropTextFile(const QUrl& url);

public slots:
    void customMenuRequested(QPoint pos);
    void write2pdf();

};
