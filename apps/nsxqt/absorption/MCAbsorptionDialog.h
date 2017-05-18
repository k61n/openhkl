#ifndef NSXQT_MCABSORPTIONDIALOG_H
#define NSXQT_MCABSORPTIONDIALOG_H

#include <memory>

#include <QDialog>

class QWidget;

namespace Ui {
class MCAbsorptionDialog;
}

namespace nsx{
class Experiment;
}

class MCAbsorptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MCAbsorptionDialog(std::shared_ptr<nsx::Experiment> experiment, QWidget *parent = 0);
    ~MCAbsorptionDialog();

private slots:
    void on_pushButton_run_pressed();

private:
    Ui::MCAbsorptionDialog *ui;
    std::shared_ptr<nsx::Experiment> _experiment;
};

#endif // NSXQT_MCABSORPTIONDIALOG_H
