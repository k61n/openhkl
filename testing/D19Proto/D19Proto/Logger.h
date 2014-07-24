#ifndef LOGGER_H
#define LOGGER_H

#include <sstream>
#include <string>
#include <unordered_map>

#include <QTextEdit>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QImageReader>

class Logger : public QTextEdit
{
public:

    enum LEVEL {DEBUG,INFO,WARNING,ERROR};

    static std::unordered_map<LEVEL,std::string, std::hash<int>> Levels;

    Logger(QWidget* parent=0);
    virtual ~Logger();

    bool canInsertFromMimeData(const QMimeData* source) const;

    void insertFromMimeData(const QMimeData* source);
    std::ostringstream& log(LEVEL level=INFO);

    void flush();

private:

    void dropImage(const QUrl& url, const QImage& image);

    void dropTextFile(const QUrl& url);

    std::ostringstream os;
};
#endif // LOGGER_H
