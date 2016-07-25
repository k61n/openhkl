#ifndef DIALOGMCABSORPTION_H
#define DIALOGMCABSORPTION_H

#include <memory>

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
    explicit DialogMCAbsorption(std::shared_ptr<SX::Instrument::Experiment> experiment, QWidget *parent = 0);
    ~DialogMCAbsorption();

private slots:
    void on_pushButton_run_pressed();

private:
    Ui::DialogMCAbsorption *ui;
    std::shared_ptr<SX::Instrument::Experiment> _experiment;
};

#endif // DIALOGMCABSORPTION_H
