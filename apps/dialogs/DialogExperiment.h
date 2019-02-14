#pragma once

#include <QDialog>

class Widget;

namespace Ui {
class DialogExperiment;
}

class DialogExperiment : public QDialog
{
    Q_OBJECT

public:
    explicit DialogExperiment(QWidget *parent = 0);
    ~DialogExperiment();

    QString getExperimentName() const;
    QString getInstrumentName() const;


private:
    Ui::DialogExperiment *ui;
};
