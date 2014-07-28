/********************************************************************************
** Form generated from reading UI file 'plotter1d.ui'
**
** Created: Mon Jul 28 00:08:13 2014
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
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_Plotter1D
{
public:
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QCustomPlot *plot;

    void setupUi(QDialog *Plotter1D)
    {
        if (Plotter1D->objectName().isEmpty())
            Plotter1D->setObjectName(QString::fromUtf8("Plotter1D"));
        Plotter1D->resize(625, 493);
        horizontalLayoutWidget = new QWidget(Plotter1D);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(10, 10, 601, 471));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        plot = new QCustomPlot(horizontalLayoutWidget);
        plot->setObjectName(QString::fromUtf8("plot"));
        plot->setMaximumSize(QSize(702, 300));

        horizontalLayout->addWidget(plot);


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
