#ifndef ADDELEMENTDIALOG_H
#define ADDELEMENTDIALOG_H

#include <QDialog>

#include "DragElementModel.h"

namespace Ui
{
    class ElementManagerDialog;
}

namespace SX
{
namespace Chemistry
{
    class ElementManager;
    class IsotopeManager;
}
}

class ElementManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ElementManagerDialog(QWidget *parent = 0);
    ~ElementManagerDialog();

private slots:

    void on_cancelButton_clicked();
    void on_saveButton_clicked();

private:
    Ui::ElementManagerDialog *ui;
    SX::Chemistry::ElementManager* _elementMgr;
    SX::Chemistry::IsotopeManager* _isotopeMgr;
    DragElementModel* _model;

};

#endif // ADDELEMENTDIALOG_H
