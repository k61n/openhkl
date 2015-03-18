#ifndef DIALOGMCABSORPTION_H
#define DIALOGMCABSORPTION_H

#include <QDialog>

namespace Ui {
class DialogMCAbsorption;
}

namespace SX{
    namespace Instrument{
        class Experiment;
    }
}

class DialogMCAbsorption : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMCAbsorption(SX::Instrument::Experiment* experiment, QWidget *parent = 0);
    ~DialogMCAbsorption();

private slots:
    void on_pushButton_run_pressed();

private:
    SX::Instrument::Experiment* _experiment;
    Ui::DialogMCAbsorption *ui;
};

#endif // DIALOGMCABSORPTION_H
