#ifndef RESOLUTIONCUTOFFDIALOG_H
#define RESOLUTIONCUTOFFDIALOG_H

#include <QDialog>

namespace Ui {
class ResolutionCutoffDialog;
}

class ResolutionCutoffDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResolutionCutoffDialog(QWidget *parent = 0);
    ~ResolutionCutoffDialog();

    double dMin();
    double dMax();

private:
    Ui::ResolutionCutoffDialog *ui;
};

#endif // RESOLUTIONCUTOFFDIALOG_H
