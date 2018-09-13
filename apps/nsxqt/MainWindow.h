#pragma once

#include <string>

#include <QGraphicsScene>
#include <QMainWindow>
#include <QMenu>
#include <QProgressBar>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/UtilsTypes.h>

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

    static MainWindow* create(QWidget* parent=nullptr);

    static MainWindow* Instance();

    ~MainWindow();

    void setColorMap(const std::string& name);

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
    void on_action_display_isotopes_database_triggered();
    void on_checkBox_AspectRatio_toggled(bool checked);

    void on_actionLogarithmic_Scale_triggered(bool checked);

    void on_actionDraw_peak_integration_area_triggered(bool checked);
     

public slots:
    void plotPeak(nsx::sptrPeak3D);
    void plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&);
    void setInspectorWidget(QWidget*);

private slots:

    void slotChangeSelectedData(nsx::sptrDataSet, int frame);

    void slotChangeSelectedPeak(nsx::sptrPeak3D peak);

    void slotChangeSelectedFrame(int selected_frame);

private:

    MainWindow(QWidget *parent = 0);

private:

    static MainWindow *_instance;

    Ui::MainWindow* _ui;

    SessionModel* _session;

    std::string _colormap;
};
