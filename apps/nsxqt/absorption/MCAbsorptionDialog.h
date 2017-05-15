#ifndef MCABSORPTIONDIALOG_H
#define MCABSORPTIONDIALOG_H

#include <memory>

#include <QDialog>

namespace Ui {
class MCAbsorptionDialog;
}

namespace nsx{
    namespace Instrument{
        class Experiment;
    }
}

class MCAbsorptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MCAbsorptionDialog(std::shared_ptr<nsx::Instrument::Experiment> experiment, QWidget *parent = 0);
    ~MCAbsorptionDialog();

private slots:
    void on_pushButton_run_pressed();

private:
    Ui::MCAbsorptionDialog *ui;
    std::shared_ptr<nsx::Instrument::Experiment> _experiment;
};

#endif // MCABSORPTIONDIALOG_H
