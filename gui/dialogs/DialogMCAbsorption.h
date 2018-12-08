#pragma once

#include <QDialog>

#include "models/SessionModel.h"

class QWidget;

namespace Ui {
class DialogMCAbsorption;
}

class ExperimentItem;

class DialogMCAbsorption: public QDialog
{
    Q_OBJECT

public:
    explicit DialogMCAbsorption(ExperimentItem* experiment_item, QWidget *parent = 0);
    ~DialogMCAbsorption();

private slots:
    void on_pushButton_run_pressed();

private:
    Ui::DialogMCAbsorption *ui;
    ExperimentItem* _experiment_item;
};
