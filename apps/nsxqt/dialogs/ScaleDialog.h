// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXQT_SCALEDIALOG_H
#define NSXQT_SCALEDIALOG_H

#include <vector>

#include <Eigen/Core>

#include <QDialog>

namespace Ui {
class ScaleDialog;
}

namespace nsx {
class Peak3D;
}

class Widget;

class ScaleDialog : public QDialog {
    Q_OBJECT

public:
    explicit ScaleDialog(const std::vector<std::vector<nsx::Peak3D*>>& peaks, QWidget *parent = 0);
    ~ScaleDialog();

    void buildPlot();
    void buildScalePlot();

    void calculateRFactors();

    void resetScale();
    void setScale();

    void refineScale();

private slots:
    void on_redrawButton_clicked();

    void on_pushButton_clicked();

private:
    double getScale(double z);

    Ui::ScaleDialog *ui;
    std::vector<std::vector<nsx::Peak3D*>> _peaks;
    std::vector<double> _averages;

    Eigen::VectorXd _scaleParams;

    int _values;
    int _numFrames;

    double _Rmerge, _Rmeas, _Rpim;
};
#endif // NSXQT_SCALEDIALOG_H
