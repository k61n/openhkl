#include "Logger.h"

#include <boost/date_time/posix_time/posix_time.hpp>

std::unordered_map<Logger::LEVEL,std::string, std::hash<int> > Logger::Levels = { {Logger::LEVEL::DEBUG,"DEBUG"},
                                                                                  {Logger::LEVEL::INFO,"INFO"},
                                                                                  {Logger::LEVEL::WARNING,"WARNING"},
                                                                                  {Logger::LEVEL::ERROR,"ERROR"} };

Logger::Logger(QWidget* parent):QTextEdit(parent)
{

}

bool Logger::canInsertFromMimeData(const QMimeData* source) const
{
    return source->hasImage() || source->hasUrls() ||
        QTextEdit::canInsertFromMimeData(source);
}
Logger::~Logger()
{

}

void Logger::insertFromMimeData(const QMimeData* source)
{
    if (source->hasImage())
    {
        static int i = 1;
        QUrl url(QString("dropped_image_%1").arg(i++));
        dropImage(url, qvariant_cast<QImage>(source->imageData()));
    }
    else if (source->hasUrls())
    {
        foreach (QUrl url, source->urls())
        {
            QFileInfo info(url.toLocalFile());
            if (QImageReader::supportedImageFormats().contains(info.suffix().toLower().toLatin1()))
                dropImage(url, QImage(info.filePath()));
            else
                dropTextFile(url);
        }
    }
    else
    {
        QTextEdit::insertFromMimeData(source);
    }
}

std::ostringstream& Logger::log(Logger::LEVEL level)
{
    boost::posix_time::ptime now=boost::posix_time::second_clock::local_time();

    switch(level)
    {
    case(DEBUG):
        setTextColor( QColor( "green" ) );
        break;
    case(INFO):
        setTextColor( QColor( "blue" ) );
        break;
    case(WARNING):
        setTextColor( QColor( "orange" ) );
        break;

    case(ERROR):
        setTextColor( QColor( "red" ) );
        break;
    };

    os << boost::posix_time::to_simple_string(now).c_str() << " (" << Levels[level] << ") --> ";

    return os;
}

void Logger::dropImage(const QUrl& url, const QImage& image)
{
    if (!image.isNull())
    {
        document()->addResource(QTextDocument::ImageResource, url, image);
        textCursor().insertImage(url.toString());
    }
}

void Logger::dropTextFile(const QUrl& url)
{
    QFile file(url.toLocalFile());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        textCursor().insertText(file.readAll());
}

void Logger::flush()
{
    std::string s=os.str();
    QString qs=QString::fromStdString(s);
    //append(QString::fromStdString(os.str()));
    os.str("");
    os.clear();
    setTextColor(QColor("black"));
}
