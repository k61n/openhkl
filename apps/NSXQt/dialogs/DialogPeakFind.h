#ifndef DIALOGPEAKFIND_H
#define DIALOGPEAKFIND_H

#include <memory>

#include "ui_dialog_PeakFind.h"
#include <QDialog>

#include <Eigen/Core>

#include "Convolver.h"

class DialogPeakFind : public QDialog
{
    Q_OBJECT
public:
    explicit DialogPeakFind(const Eigen::MatrixXi& currentFrame, QWidget *parent = 0);

    double getConfidence();
    double getThreshold();

    std::shared_ptr<SX::Imaging::Convolver> getConvolver();

    ~DialogPeakFind();

signals:

public slots:
private slots:

    void on_filterComboBox_activated(int index);

private:
    Ui::DialogPeakFind* ui;
    Eigen::MatrixXi _currentFrame;
    std::shared_ptr<SX::Imaging::Convolver> _convolver;

};
#endif // DIALOGPEAKFIND_H
