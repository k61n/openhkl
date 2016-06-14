#ifndef DIALOGPEAKFIND_H
#define DIALOGPEAKFIND_H
#include "ui_dialog_PeakFind.h"
#include <QDialog>

#include <Eigen/Core>

class DialogPeakFind : public QDialog
{
    Q_OBJECT
public:
    explicit DialogPeakFind(const Eigen::MatrixXi& currentFrame, QWidget *parent = 0);

    double getConfidence();
    double getThreshold();


    ~DialogPeakFind();

signals:

public slots:
private slots:

    void on_filterComboBox_activated(int index);

private:
    Ui::DialogPeakFind* ui;
    Eigen::MatrixXi _currentFrame;

};
#endif // DIALOGPEAKFIND_H
