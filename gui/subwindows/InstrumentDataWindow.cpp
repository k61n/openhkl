#include "InstrumentDataWindow.h"

#include <QTableWidget>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QStringList>
#include <QTextEdit>
#include <QPushButton> 
#include <QLabel>
#include <iostream>

InstrumentDataWindow::InstrumentDataWindow(QWidget* parent) 
: QDialog(parent)
{
    setModal(false); 
    QVBoxLayout* lay = new QVBoxLayout(this);
    QHBoxLayout* head_layout = new QHBoxLayout( );


   
    QLabel* lab = new QLabel();
    lab->setText(  QString("Instrument"));
   
    _instruments = new QComboBox();
    _instruments->setMaximumSize(QSize(300,50));
    _instruments->addItem("aaa");
    _instruments->addItem("bbb");
    head_layout->addWidget(lab);
    head_layout->addWidget(_instruments);

    lay->addLayout(head_layout);

    _table = new QTableWidget(0,2);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setHorizontalHeaderLabels(QStringList{"Key", "Value" });
    _table->resizeColumnsToContents();
    _table->verticalHeader()->setVisible(false);

    lay->addWidget(_table);


   

}             
void InstrumentDataWindow::refreshAll()
{}