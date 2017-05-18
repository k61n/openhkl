#ifndef NSXQT_RESOLUTIONCUTOFFDIALOG_H
#define NSXQT_RESOLUTIONCUTOFFDIALOG_H

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

#endif // NSXQT_RESOLUTIONCUTOFFDIALOG_H
