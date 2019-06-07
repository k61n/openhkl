#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QPrinter>

#include "NoteBook.h"

NoteBook::NoteBook(QWidget* parent) : QTextEdit(parent)
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(
        this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested(QPoint)));
}

bool NoteBook::canInsertFromMimeData(const QMimeData* source) const
{
    return source->hasImage() || source->hasUrls() || QTextEdit::canInsertFromMimeData(source);
}

NoteBook::~NoteBook() {}

void NoteBook::insertFromMimeData(const QMimeData* source)
{
    if (source->hasImage()) {
        static int i = 1;
        QUrl url(QString("dropped_image_%1").arg(i++));
        dropImage(url, qvariant_cast<QImage>(source->imageData()));
    } else if (source->hasUrls()) {
        foreach (QUrl url, source->urls()) {
            QFileInfo info(url.toLocalFile());
            if (QImageReader::supportedImageFormats().contains(info.suffix().toLower().toLatin1()))
                dropImage(url, QImage(info.filePath()));
            else
                dropTextFile(url);
        }
    } else {
        QTextEdit::insertFromMimeData(source);
    }
}

void NoteBook::dropImage(const QUrl& url, const QImage& image)
{
    if (!image.isNull()) {
        document()->addResource(QTextDocument::ImageResource, url, image);
        textCursor().insertImage(url.toString());
    }
}

void NoteBook::dropTextFile(const QUrl& url)
{
    QFile file(url.toLocalFile());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        textCursor().insertText(file.readAll());
}

void NoteBook::customMenuRequested(QPoint pos)
{
    QMenu* menu = this->createStandardContextMenu();
    menu->addSeparator();
    QAction* write2pdf = menu->addAction(QIcon(":/resources/writeToPDFIcon.png"), "Save to pdf");
    write2pdf->setShortcut(QKeySequence::Save);
    menu->popup(viewport()->mapToGlobal(pos));
    connect(write2pdf, SIGNAL(triggered()), this, SLOT(write2pdf()));
}

void NoteBook::write2pdf()
{
    QString fileName =
        QFileDialog::getSaveFileName(this, tr("save as pdf"), "", tr("PDF Files (*.pdf)"));
    QPrinter printer;
    printer.setPageSize(QPrinter::Letter);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    this->print(&printer);
}

void NoteBook::printLogMessage(const std::string& message)
{
    append(QString::fromStdString(message));
}
