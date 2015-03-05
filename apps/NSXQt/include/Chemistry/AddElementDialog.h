#ifndef ADDELEMENTDIALOG_H
#define ADDELEMENTDIALOG_H

#include <QDialog>

#include "DragElementModel.h"

namespace Ui
{
    class AddElementDialog;
}

namespace SX
{
namespace Chemistry
{
    class ElementManager;
    class IsotopeManager;
}
}

class AddElementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddElementDialog(QWidget *parent = 0);
    ~AddElementDialog();

private slots:

    void on_cancelButton_clicked();
    void on_saveButton_clicked();

private:
    Ui::AddElementDialog *ui;
    SX::Chemistry::ElementManager* _elementMgr;
    SX::Chemistry::IsotopeManager* _isotopeMgr;
    DragElementModel* _model;

};

#endif // ADDELEMENTDIALOG_H
