#include "dialogpeaklist.h"
#include "ui_dialogpeaklist.h"

DialogPeakList::DialogPeakList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPeakList)
{
    ui->setupUi(this);
}

DialogPeakList::~DialogPeakList()
{
    delete ui;
}
