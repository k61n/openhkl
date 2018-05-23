#ifndef NSXQT_MAINWINDOW_H
#define NSXQT_MAINWINDOW_H

#include <map>
#include <memory>

#include <QGraphicsScene>
#include <QMainWindow>
#include <QMenu>
#include <QProgressBar>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/UtilsTypes.h>

#include "JobHandler.h"
#include "ProgressView.h"

class QListWidgetItem;
class PlottableGraphicsItem;
class SessionModel;

namespace Ui {
class MainWindow;
}

class QProgressDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    Ui::MainWindow* getUI() const;

    ~MainWindow();

    void findPeaks();

signals:
    void plotDetectorData(nsx::DataSet*,int frame);

private slots:
    void on_actionAbout_triggered();

    void on_actionPixel_position_triggered();
    void on_actionGamma_Nu_triggered();
    void on_action2_Theta_triggered();
    void on_actionD_spacing_triggered();
    void on_actionLogger_triggered();
    void on_action1D_Peak_Ploter_triggered();
    void on_actionProperty_triggered();
    void updatePlot(PlottableGraphicsItem* cutter);
    void on_actionFrom_Sample_triggered();
    void on_actionBehind_Detector_triggered();
    void on_actionShow_labels_triggered(bool checked);
    void on_action_display_isotopes_database_triggered();
    void on_actionConvert_to_HDF5_triggered();
    void on_actionH_k_l_triggered();
    void on_checkBox_AspectRatio_toggled(bool checked);

    void on_actionLogarithmic_Scale_triggered(bool checked);

    void on_actionDraw_peak_integration_area_triggered(bool checked);
     

public slots:
    void changeData(nsx::sptrDataSet);
    void plotPeak(nsx::sptrPeak3D);
    void plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&);
    void setInspectorWidget(QWidget*);

private:
    Ui::MainWindow* _ui;

    nsx::sptrDataSet _currentData;

    nsx::sptrProgressHandler _progressHandler;
    nsx::sptrPeakFinder _peakFinder;

    SessionModel* _session;
};

#endif // MAINWINDOW_H
