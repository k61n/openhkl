#ifndef ISOTOPEDATABASEDIALOG_H
#define ISOTOPEDATABASEDIALOG_H

#include <QDialog>

#include <string>

namespace Ui {
class IsotopeDatabaseDialog;
}

class IsotopeDatabaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IsotopeDatabaseDialog(QWidget *parent = 0);

    ~IsotopeDatabaseDialog();

private:
    Ui::IsotopeDatabaseDialog *ui;
};

#endif // ISOTOPEDATABASEDIALOG_H
