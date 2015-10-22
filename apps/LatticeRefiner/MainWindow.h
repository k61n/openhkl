#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Diffractometer.h"
#include "Peak3D.h"
#include <vector>
#include <utility>

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
    void test();
private:
    Ui::MainWindow *ui;
    //! List of peaks and whether they are use in the refinement or not (true/false)
    std::vector<std::pair<SX::Crystal::Peak3D,bool>> _peaks;
};

#endif // MAINWINDOW_H
