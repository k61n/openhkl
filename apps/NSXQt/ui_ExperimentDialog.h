/********************************************************************************
** Form generated from reading UI file 'ExperimentDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXPERIMENTDIALOG_H
#define UI_EXPERIMENTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ExperimentDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QTextEdit *textEdit;
    QWidget *widget1;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QComboBox *comboBox;

    void setupUi(QDialog *ExperimentDialog)
    {
        if (ExperimentDialog->objectName().isEmpty())
            ExperimentDialog->setObjectName(QStringLiteral("ExperimentDialog"));
        ExperimentDialog->setWindowModality(Qt::ApplicationModal);
        ExperimentDialog->resize(500, 130);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ExperimentDialog->sizePolicy().hasHeightForWidth());
        ExperimentDialog->setSizePolicy(sizePolicy);
        ExperimentDialog->setMinimumSize(QSize(500, 130));
        ExperimentDialog->setMaximumSize(QSize(500, 130));
        buttonBox = new QDialogButtonBox(ExperimentDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(314, 80, 176, 27));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(false);
        widget = new QWidget(ExperimentDialog);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(10, 21, 252, 32));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(label);

        textEdit = new QTextEdit(widget);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
        textEdit->setSizePolicy(sizePolicy2);
        textEdit->setMaximumSize(QSize(200, 30));

        horizontalLayout->addWidget(textEdit);

        widget1 = new QWidget(ExperimentDialog);
        widget1->setObjectName(QStringLiteral("widget1"));
        widget1->setGeometry(QRect(318, 22, 173, 29));
        horizontalLayout_2 = new QHBoxLayout(widget1);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(widget1);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        comboBox = new QComboBox(widget1);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        horizontalLayout_2->addWidget(comboBox);


        retranslateUi(ExperimentDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ExperimentDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ExperimentDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ExperimentDialog);
    } // setupUi

    void retranslateUi(QDialog *ExperimentDialog)
    {
        ExperimentDialog->setWindowTitle(QApplication::translate("ExperimentDialog", "New Experiment", 0));
#ifndef QT_NO_TOOLTIP
        ExperimentDialog->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("ExperimentDialog", "Name:", 0));
        label_2->setText(QApplication::translate("ExperimentDialog", "Instrument:", 0));
    } // retranslateUi

};

namespace Ui {
    class ExperimentDialog: public Ui_ExperimentDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXPERIMENTDIALOG_H
