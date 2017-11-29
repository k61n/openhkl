#ifndef NSXQT_MCABSORPTIONDIALOG_H
#define NSXQT_MCABSORPTIONDIALOG_H

#include <QDialog>

#include <nsxlib/InstrumentTypes.h>
#include "models/SessionModel.h"

class QWidget;

namespace Ui {
class MCAbsorptionDialog;
}

class MCAbsorptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MCAbsorptionDialog(std::shared_ptr<SessionModel> session, nsx::sptrExperiment experiment, QWidget *parent = 0);
    ~MCAbsorptionDialog();

private slots:
    void on_pushButton_run_pressed();

private:
    Ui::MCAbsorptionDialog *ui;
    nsx::sptrExperiment _experiment;
    std::shared_ptr<SessionModel> _session;
};

#endif // NSXQT_MCABSORPTIONDIALOG_H
