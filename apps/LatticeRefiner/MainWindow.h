#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <utility>
#include <vector>

#include <QMainWindow>

#include <nsxlib/crystal/LatticeMinimizer.h>
#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/instrument/Diffractometer.h>

namespace Ui {
class MainWindow;
}

class MainWindow: public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_actionOpen_reflections_triggered();
    void on_pushButton_refine_clicked();
    void selectPeaks();
    void on_comboBox_diffractometer_currentIndexChanged(const QString& diffractometerName);
    // void on_actionShow_calculated_peak_positions_triggered();
    // void on_actionShow_calculated_peak_positions_triggered(bool checked);

private:
    Ui::MainWindow *ui;
    //! List of peaks and whether they are use in the refinement or not (true/false)
    std::vector<std::pair<SX::Crystal::Peak3D,bool>> _peaks;
    SX::Crystal::LatticeMinimizer _minimizer;
    std::shared_ptr<SX::Instrument::Diffractometer> _diffractometer;
};

#endif // MAINWINDOW_H
