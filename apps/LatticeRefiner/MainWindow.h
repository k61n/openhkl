#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Diffractometer.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_reflections_triggered();

    void on_pushButton_refine_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
