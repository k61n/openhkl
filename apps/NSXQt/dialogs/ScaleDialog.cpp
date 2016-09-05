// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include "ScaleDialog.h"
#include "ui_ScaleDialog.h"

#include <QImage>
#include <QTreeView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QList>
#include <QString>

#include <QModelIndex>

#include <Eigen/Core>
#include <QDebug>

#include <iostream>

#include <QMessageBox>

#include "Sample.h"
#include "SpaceGroup.h"
#include "SpaceGroupSymbols.h"
#include "IData.h"

using namespace std;

ScaleDialog::ScaleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScaleDialog)
{
    ui->setupUi(this);
}

ScaleDialog::~ScaleDialog()
{
    delete ui;
    // this should be handled by Qt. check with valgrind?
    // delete _peakFindModel;
}
