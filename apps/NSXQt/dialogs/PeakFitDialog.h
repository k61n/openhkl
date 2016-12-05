#ifndef PEAKFITDIALOG_H
#define PEAKFITDIALOG_H

#include <QDialog>

namespace Ui {
class PeakFitDialog;
}

class PeakFitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PeakFitDialog(QWidget *parent = 0);
    ~PeakFitDialog();

private:
    Ui::PeakFitDialog *ui;
};

#endif // PEAKFITDIALOG_H
