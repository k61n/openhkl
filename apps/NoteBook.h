//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/NoteBook.h
//! @brief     Defines class NoteBook
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QImageReader>
#include <QMimeData>
#include <QTextEdit>

/*
 * A QtextEdit inherited class in which image and text can be dragged and
 * dropped in.
 * */
class NoteBook : public QTextEdit {
    Q_OBJECT
 public:
    NoteBook(QWidget* parent = 0);
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
