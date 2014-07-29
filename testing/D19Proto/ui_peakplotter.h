/********************************************************************************
** Form generated from reading UI file 'peakplotter.ui'
**
** Created: Tue Jul 29 23:11:19 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PEAKPLOTTER_H
#define UI_PEAKPLOTTER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_PeakPlotter
{
public:
    QCustomPlot *widget;

    void setupUi(QDialog *PeakPlotter)
    {
        if (PeakPlotter->objectName().isEmpty())
            PeakPlotter->setObjectName(QString::fromUtf8("PeakPlotter"));
        PeakPlotter->resize(719, 402);
        widget = new QCustomPlot(PeakPlotter);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(10, 10, 691, 371));

        retranslateUi(PeakPlotter);

        QMetaObject::connectSlotsByName(PeakPlotter);
    } // setupUi

    void retranslateUi(QDialog *PeakPlotter)
    {
        PeakPlotter->setWindowTitle(QApplication::translate("PeakPlotter", "Peak plotter", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PeakPlotter: public Ui_PeakPlotter {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PEAKPLOTTER_H
