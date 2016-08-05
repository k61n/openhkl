#ifndef ADDELEMENTDIALOG_H
#define ADDELEMENTDIALOG_H

#include <QDialog>

#include "DragElementModel.h"
#include "ElementManager.h"

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

    //! Constructor
    explicit ElementManagerDialog(QWidget *parent = 0);

    //! Destructor
    ~ElementManagerDialog();

private slots:

    //! Set the newly built element to the registry
    void on_setButton_clicked();
    //! Save the registry in its current state to the elements XML database
    void on_saveButton_clicked();
    //! Cloes the leaving the elements registry in its current state
    void on_okButton_clicked();
    //! Deletes one element from the registry
    void deleteElement();
    //! Removes one of the isotopes in use in the new element
    void removeIsotope();

private:
    Ui::ElementManagerDialog *ui;
    SX::Chemistry::ElementManager* _elementMgr;
    SX::Chemistry::IsotopeManager* _isotopeMgr;
    DragElementModel* _model;

};

#endif // ADDELEMENTDIALOG_H
