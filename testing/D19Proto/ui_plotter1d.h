/********************************************************************************
** Form generated from reading UI file 'plotter1d.ui'
**
** Created: Sun Jul 27 00:14:52 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLOTTER1D_H
#define UI_PLOTTER1D_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_Plotter1D
{
public:
    QCustomPlot *plot;

    void setupUi(QDialog *Plotter1D)
    {
        if (Plotter1D->objectName().isEmpty())
            Plotter1D->setObjectName(QString::fromUtf8("Plotter1D"));
        Plotter1D->resize(625, 493);
        plot = new QCustomPlot(Plotter1D);
        plot->setObjectName(QString::fromUtf8("plot"));
        plot->setGeometry(QRect(70, 60, 401, 321));

        retranslateUi(Plotter1D);

        QMetaObject::connectSlotsByName(Plotter1D);
    } // setupUi

    void retranslateUi(QDialog *Plotter1D)
    {
        Plotter1D->setWindowTitle(QApplication::translate("Plotter1D", "Dialog", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Plotter1D: public Ui_Plotter1D {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLOTTER1D_H
