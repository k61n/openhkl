#ifndef NSXQT_MCABSORPTIONDIALOG_H
#define NSXQT_MCABSORPTIONDIALOG_H

#include <QDialog>

#include <nsxlib/InstrumentTypes.h>

class QWidget;

namespace Ui {
class MCAbsorptionDialog;
}

class MCAbsorptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MCAbsorptionDialog(nsx::sptrExperiment experiment, QWidget *parent = 0);
    ~MCAbsorptionDialog();

private slots:
    void on_pushButton_run_pressed();

private:
    Ui::MCAbsorptionDialog *ui;
    nsx::sptrExperiment _experiment;
};

#endif // NSXQT_MCABSORPTIONDIALOG_H
