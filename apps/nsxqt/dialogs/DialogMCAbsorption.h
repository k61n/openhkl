#pragma once

#include <QDialog>

#include <nsxlib/InstrumentTypes.h>
#include "models/SessionModel.h"

class QWidget;

namespace Ui {
class DialogMCAbsorption;
}

class DialogMCAbsorption: public QDialog
{
    Q_OBJECT

public:
    explicit DialogMCAbsorption(SessionModel* session, nsx::sptrExperiment experiment, QWidget *parent = 0);
    ~DialogMCAbsorption();

private slots:
    void on_pushButton_run_pressed();

private:
    Ui::DialogMCAbsorption *ui;
    nsx::sptrExperiment _experiment;
    SessionModel* _session;
};
