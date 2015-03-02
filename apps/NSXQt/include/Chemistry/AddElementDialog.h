#ifndef ADDELEMENTDIALOG_H
#define ADDELEMENTDIALOG_H

#include <QDialog>

#include "IsotopeManager.h"
#include "DragElementModel.h"

namespace Ui {
class AddElementDialog;
}

class AddElementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddElementDialog(QWidget *parent = 0);
    ~AddElementDialog();

public slots:

    void dropIsotope(QModelIndex,QModelIndex);


private:
    Ui::AddElementDialog *ui;
    SX::Chemistry::IsotopeManager* _isotopeMgr;
    DragElementModel* _model;

};

#endif // ADDELEMENTDIALOG_H
