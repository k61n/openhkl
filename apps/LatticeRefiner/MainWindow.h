#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>

#include "Diffractometer.h"
#include "Peak3D.h"

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

private:
    Ui::MainWindow *ui;
    std::vector<SX::Crystal::Peak3D> _peaks;

};

#endif // MAINWINDOW_H
