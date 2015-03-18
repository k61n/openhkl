#ifndef NUMORSCONVERSIONDIALOG_H
#define NUMORSCONVERSIONDIALOG_H

#include <QDialog>

namespace Ui {
class NumorsConversionDialog;
}

class NumorsConversionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NumorsConversionDialog(QWidget *parent = 0);
    ~NumorsConversionDialog();

private slots:
    void on_pushButton_convert_clicked();

    void on_pushButton_cancel_clicked();

    void on_pushButton_browse_clicked();

private:
    Ui::NumorsConversionDialog *ui;
};

#endif // NUMORSCONVERSIONDIALOG_H
