//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/views.cpp
//! @brief     Implements enhanced view widgets QcrIcon, QcrLineDisplay
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "qcr/widgets/views.h"
#include "qcr/base/string_ops.h"

QcrIcon::QcrIcon(const QString& fileName)
    : QcrBase{"icon:"+fileName}
{
    int h = sizeHint().height();
    setPixmap(QIcon{fileName}.pixmap(QSize{h, h}));
}

QcrLineDisplay::QcrLineDisplay(std::function<QString()> freshText)
    : QcrBase{"lineDisplay"}
{
    setReadOnly(true);
    setRemake( [this, freshText]() { setText( freshText() ); } );
}

QcrLineDisplay::QcrLineDisplay(int ndigits, bool withDot)
    : QcrBase{"lineDisplay"}
{
    strOp::setWidth(this, ndigits, withDot);
    setText("???");
}
