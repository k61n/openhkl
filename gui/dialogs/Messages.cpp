//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/Messages.cpp
//! @brief     Implements class AboutBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING) //! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/Messages.h"

#include "gui/MainWin.h"

#include <QApplication>
#include <QDate>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolTip>
#include <QVBoxLayout>

AboutBox::AboutBox() : QDialog{gGui, Qt::Dialog}
{
    auto* vb = new QVBoxLayout;
    auto* hb = new QHBoxLayout;
    // auto* logo = new QLabel;
    auto* info = new QLabel;
    auto* dbbox = new QDialogButtonBox{QDialogButtonBox::Ok};

    setWindowTitle(QString("About %1").arg(qApp->applicationName()));

    // vertical layout
    setLayout(vb);
    vb->setSpacing(12);
    vb->setSizeConstraint(QLayout::SetFixedSize);

    // logo and info
    vb->addLayout(hb);
    hb->setSpacing(12);
    // logo->setPixmap(QPixmap(":/icon/retroStier")
    // .scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    // hb->addWidget(logo);

#ifdef __x86_64__
    QString arch = "(64b)";
#else
    QString arch = "";
#endif
    info->setText(QString("<h4>%1 version %2 %3</h4>"
                          "<p><a href=\"%4\">Git repository</a></p>"
                          "<p>Git branch %5, commit %6</p>"
                          "<p>%7</p>"
                          "<p>Copyright: Forschungszentrum Jülich GmbH %8</p>")
                      .arg(qApp->applicationName())
                      .arg(qApp->applicationVersion())
                      .arg(arch)
                      .arg(REPO_URL)
                      .arg(GIT_BRANCH)
                      .arg(COMMIT_HASH)
                      .arg(APPLICATION_CLAIM)
                      .arg(QDate::currentDate().toString("yyyy")));
    info->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    info->setTextFormat(Qt::RichText);
    info->setTextInteractionFlags(Qt::TextBrowserInteraction);
    info->setOpenExternalLinks(true);
    /* info->setOpenExternalLinks(true); */
#ifdef Q_OS_MAC
    // a smaller font (a hint found in Qt source code)
    info->setFont(QToolTip::font());
#endif
    hb->addWidget(info);

    vb->addWidget(dbbox);

    connect(dbbox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}
