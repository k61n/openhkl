#include <algorithm>
#include <iterator>

#include <QAbstractTableModel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QDebug>
#include <QStyleFactory>

#include "ConstraintsModel.h"
#include "CheckableComboBox.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _models()
{
    ui->setupUi(this);

    ui->checkBox_alpha->setText(QString((QChar) 0x03B1));
    ui->checkBox_beta->setText(QString((QChar) 0x03B2));
    ui->checkBox_gamma->setText(QString((QChar) 0x03B3));

    ConstraintsModel* model;

    QStandardItem* item;

    ui->comboBox_a->setStyle(QStyleFactory::create("Windows"));
    ui->comboBox_b->setStyle(QStyleFactory::create("Windows"));
    ui->comboBox_c->setStyle(QStyleFactory::create("Windows"));
    ui->comboBox_alpha->setStyle(QStyleFactory::create("Windows"));
    ui->comboBox_beta->setStyle(QStyleFactory::create("Windows"));
    ui->comboBox_gamma->setStyle(QStyleFactory::create("Windows"));

    model = new ConstraintsModel();
    item = new QStandardItem("b");
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked,Qt::CheckStateRole);
    model->setItem(0,0,item);
    item = new QStandardItem("c");
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked,Qt::CheckStateRole);
    model->setItem(1,0,item);
    ui->comboBox_a->setModel(model);
    _models.push_back(model);
    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slot_changed(const QModelIndex&, const QModelIndex&)));

    model = new ConstraintsModel();
    item = new QStandardItem();
    item->setText("a");
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked,Qt::CheckStateRole);
    model->setItem(0,0,item);
    item = new QStandardItem();
    item->setText("c");
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked,Qt::CheckStateRole);
    model->setItem(1,0,item);
    ui->comboBox_b->setModel(model);
    _models.push_back(model);
    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slot_changed(const QModelIndex&, const QModelIndex&)));

    model = new ConstraintsModel();
    item = new QStandardItem();
    item->setText("a");
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked,Qt::CheckStateRole);
    model->setItem(0,0,item);
    item = new QStandardItem();
    item->setText("b");
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked,Qt::CheckStateRole);
    model->setItem(1,0,item);
    ui->comboBox_c->setModel(model);
    _models.push_back(model);
    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slot_changed(const QModelIndex&, const QModelIndex&)));

    model = new ConstraintsModel();
    item = new QStandardItem();
    item->setText(QString((QChar) 0x03B2));
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked,Qt::CheckStateRole);
    model->setItem(0,0,item);
    item = new QStandardItem();
    item->setText(QString((QChar) 0x03B3));
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked,Qt::CheckStateRole);
    model->setItem(1,0,item);
    ui->comboBox_alpha->setModel(model);
    _models.push_back(model);
    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slot_changed(const QModelIndex&, const QModelIndex&)));

    model = new ConstraintsModel();
    item = new QStandardItem();
    item->setText(QString((QChar) 0x03B1));
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked,Qt::CheckStateRole);
    model->setItem(0,0,item);
    item = new QStandardItem();
    item->setText(QString((QChar) 0x03B3));
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked,Qt::CheckStateRole);
    model->setItem(1,0,item);
    ui->comboBox_beta->setModel(model);
    _models.push_back(model);
    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slot_changed(const QModelIndex&, const QModelIndex&)));

    model = new ConstraintsModel();
    item = new QStandardItem();
    item->setText(QString((QChar) 0x03B2));
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked,Qt::CheckStateRole);
    model->setItem(0,0,item);
    item = new QStandardItem();
    item->setText(QString((QChar) 0x03B3));
    item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked,Qt::CheckStateRole);
    model->setItem(1,0,item);
    ui->comboBox_gamma->setModel(model);
    _models.push_back(model);
    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slot_changed(const QModelIndex&, const QModelIndex&)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_changed(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    auto it=std::find(_models.begin(),_models.end(),topLeft.model());
    if (it!=_models.end())
    {
        qDebug()<<std::distance(_models.begin(),it)<<" "<<topLeft.row();
    }

    QModelIndex idx = _models[0]->index(0,0);
    _models[0]->setData(idx,Qt::Checked,Qt::CheckStateRole);

}
