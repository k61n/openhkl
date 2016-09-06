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

#include "IData.h"
#include "Peak3D.h"



namespace Ui {
class ScaleDialog;
}

class ScaleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScaleDialog(const std::vector<std::vector<SX::Crystal::Peak3D*>>& peaks, QWidget *parent = 0);
    ~ScaleDialog();

    void buildPlot();
    void buildScalePlot();

private slots:


    void on_redrawButton_clicked();

private:
    Ui::ScaleDialog *ui;
    std::vector<std::vector<SX::Crystal::Peak3D*>> _peaks;
};
#endif // NSXTOOL_SCALEDIALOG_H_
