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



namespace Ui {
class ScaleDialog;
}

class ScaleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScaleDialog(QWidget *parent = 0);
    ~ScaleDialog();


private slots:


private:
    Ui::ScaleDialog *ui;
};
#endif // NSXTOOL_SCALEDIALOG_H_
