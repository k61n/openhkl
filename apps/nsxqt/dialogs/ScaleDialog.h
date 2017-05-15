// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_SCALEDIALOG_H_
#define NSXTOOL_SCALEDIALOG_H_

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QWidget>

#include <QTableWidgetItem>

#include <Eigen/Core>

#include <map>
#include <string>
#include <memory>

#include <nsxlib/data/IData.h>
#include <nsxlib/crystal/Peak3D.h>



namespace Ui {
class ScaleDialog;
}

class ScaleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScaleDialog(const std::vector<std::vector<nsx::Crystal::Peak3D*>>& peaks, QWidget *parent = 0);
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
    std::vector<std::vector<nsx::Crystal::Peak3D*>> _peaks;
    std::vector<double> _averages;

    Eigen::VectorXd _scaleParams;

    int _values;
    int _numFrames;

    double _Rmerge, _Rmeas, _Rpim;
};
#endif // NSXTOOL_SCALEDIALOG_H_
