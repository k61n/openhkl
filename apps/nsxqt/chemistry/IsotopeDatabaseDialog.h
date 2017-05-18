#ifndef NSXQT_ISOTOPEDATABASEDIALOG_H
#define NSXQT_ISOTOPEDATABASEDIALOG_H

#include <QDialog>

class QWidget;

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

#endif // NSXQT_ISOTOPEDATABASEDIALOG_H
