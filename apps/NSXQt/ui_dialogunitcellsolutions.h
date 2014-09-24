/********************************************************************************
** Form generated from reading UI file 'dialogunitcellsolutions.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGUNITCELLSOLUTIONS_H
#define UI_DIALOGUNITCELLSOLUTIONS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableView>

QT_BEGIN_NAMESPACE

class Ui_DialogUnitCellSolutions
{
public:
    QGridLayout *gridLayout;
    QTableView *tableView;

    void setupUi(QDialog *DialogUnitCellSolutions)
    {
        if (DialogUnitCellSolutions->objectName().isEmpty())
            DialogUnitCellSolutions->setObjectName(QStringLiteral("DialogUnitCellSolutions"));
        DialogUnitCellSolutions->resize(577, 345);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DialogUnitCellSolutions->sizePolicy().hasHeightForWidth());
        DialogUnitCellSolutions->setSizePolicy(sizePolicy);
        DialogUnitCellSolutions->setMinimumSize(QSize(577, 345));
        DialogUnitCellSolutions->setMaximumSize(QSize(577, 345));
        DialogUnitCellSolutions->setWindowOpacity(0.9);
        gridLayout = new QGridLayout(DialogUnitCellSolutions);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        tableView = new QTableView(DialogUnitCellSolutions);
        tableView->setObjectName(QStringLiteral("tableView"));

        gridLayout->addWidget(tableView, 0, 0, 1, 1);


        retranslateUi(DialogUnitCellSolutions);

        QMetaObject::connectSlotsByName(DialogUnitCellSolutions);
    } // setupUi

    void retranslateUi(QDialog *DialogUnitCellSolutions)
    {
        DialogUnitCellSolutions->setWindowTitle(QApplication::translate("DialogUnitCellSolutions", "Unit-cells", 0));
    } // retranslateUi

};

namespace Ui {
    class DialogUnitCellSolutions: public Ui_DialogUnitCellSolutions {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGUNITCELLSOLUTIONS_H
