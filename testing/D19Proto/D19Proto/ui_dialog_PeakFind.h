/********************************************************************************
** Form generated from reading UI file 'dialog_PeakFind.ui'
**
** Created: Thu Jul 24 14:27:23 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_PEAKFIND_H
#define UI_DIALOG_PEAKFIND_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DialogPeakFind
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *confidenceLabel;
    QDoubleSpinBox *confidenceSpinBox;
    QLabel *thresholdLabel;
    QDoubleSpinBox *thresholdSpinBox;

    void setupUi(QDialog *DialogPeakFind)
    {
        if (DialogPeakFind->objectName().isEmpty())
            DialogPeakFind->setObjectName(QString::fromUtf8("DialogPeakFind"));
        DialogPeakFind->setWindowModality(Qt::WindowModal);
        DialogPeakFind->resize(400, 300);
        buttonBox = new QDialogButtonBox(DialogPeakFind);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox = new QGroupBox(DialogPeakFind);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(30, 30, 321, 191));
        gridLayoutWidget = new QWidget(groupBox);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(29, 60, 231, 80));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        confidenceLabel = new QLabel(gridLayoutWidget);
        confidenceLabel->setObjectName(QString::fromUtf8("confidenceLabel"));

        gridLayout->addWidget(confidenceLabel, 1, 0, 1, 1);

        confidenceSpinBox = new QDoubleSpinBox(gridLayoutWidget);
        confidenceSpinBox->setObjectName(QString::fromUtf8("confidenceSpinBox"));
        confidenceSpinBox->setMaximum(1);
        confidenceSpinBox->setValue(0.99);

        gridLayout->addWidget(confidenceSpinBox, 1, 1, 1, 1);

        thresholdLabel = new QLabel(gridLayoutWidget);
        thresholdLabel->setObjectName(QString::fromUtf8("thresholdLabel"));

        gridLayout->addWidget(thresholdLabel, 0, 0, 1, 1);

        thresholdSpinBox = new QDoubleSpinBox(gridLayoutWidget);
        thresholdSpinBox->setObjectName(QString::fromUtf8("thresholdSpinBox"));
        thresholdSpinBox->setMinimum(1.5);
        thresholdSpinBox->setMaximum(100000);

        gridLayout->addWidget(thresholdSpinBox, 0, 1, 1, 1);


        retranslateUi(DialogPeakFind);
        QObject::connect(buttonBox, SIGNAL(accepted()), DialogPeakFind, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DialogPeakFind, SLOT(reject()));

        QMetaObject::connectSlotsByName(DialogPeakFind);
    } // setupUi

    void retranslateUi(QDialog *DialogPeakFind)
    {
        DialogPeakFind->setWindowTitle(QApplication::translate("DialogPeakFind", "Dialog", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("DialogPeakFind", "Peak Find Parameters", 0, QApplication::UnicodeUTF8));
        confidenceLabel->setText(QApplication::translate("DialogPeakFind", "Confidence : ", 0, QApplication::UnicodeUTF8));
        thresholdLabel->setText(QApplication::translate("DialogPeakFind", "Threshold (x bkg): ", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DialogPeakFind: public Ui_DialogPeakFind {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_PEAKFIND_H
