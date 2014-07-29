/********************************************************************************
** Form generated from reading UI file 'plotter1d.ui'
**
** Created: Tue Jul 29 23:11:19 2014
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
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_Plotter1D
{
public:
    QHBoxLayout *horizontalLayout;
    QCustomPlot *plot;

    void setupUi(QDialog *Plotter1D)
    {
        if (Plotter1D->objectName().isEmpty())
            Plotter1D->setObjectName(QString::fromUtf8("Plotter1D"));
        Plotter1D->resize(625, 493);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(Plotter1D->sizePolicy().hasHeightForWidth());
        Plotter1D->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(Plotter1D);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        plot = new QCustomPlot(Plotter1D);
        plot->setObjectName(QString::fromUtf8("plot"));
        sizePolicy.setHeightForWidth(plot->sizePolicy().hasHeightForWidth());
        plot->setSizePolicy(sizePolicy);
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
