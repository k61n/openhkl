#pragma once

#include <string>

#include <QGraphicsScene>
#include <QMainWindow>
#include <QMenu>
#include <QProgressBar>

#include <core/PeakList.h>
#include <core/DataSet.h>
#include <core/DataTypes.h>

#include "ProgressView.h"
#include "TaskManagerModel.h"

class DataItem;
class NSXMenu;
class PlottableGraphicsItem;
class QListWidgetItem;
class QStatusBar;
class SessionModel;

namespace Ui {
class MainWindow;
}

class QProgressDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    MainWindow(QWidget *parent=nullptr);

    ~MainWindow();

    virtual void closeEvent(QCloseEvent *event) override;

    TaskManagerModel* taskManagerModel();

public slots:

    void plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&);

    void setInspectorWidget(QWidget*);

    void onOpenPeakFinderDialog(DataItem *data_item);

    void onNewExperiment();
    void onTogglePlotterPanel();
    void onToggleMonitorPanel();
    void onToggleWidgetPropertyPanel();
    void onViewDetectorFromBehind();
    void onViewDetectorFromSample();

    void onSelectPixelPositionCursorMode();
    void onSelectGammaNuCursorMode();
    void onSelect2ThetaCursorMode();
    void onSelectDSpacingCursorMode();
    void onSelectMillerIndicesCursorMode();

    void onDisplayPeakAreas(bool flag);
    void onDisplayPeakIntegrationAreas(bool flag);
    void onDisplayPeakLabels(bool flag);

    void onSetColorMap(const std::string& color_map);

private slots:

    void updatePlot(PlottableGraphicsItem* cutter);

    void on_checkBox_AspectRatio_toggled(bool checked);

    void on_actionLogarithmic_Scale_triggered(bool checked);


    void slotChangeSelectedData(nsx::sptrDataSet, int frame);

    void slotChangeSelectedPeak(nsx::sptrPeak3D peak);

    void slotChangeSelectedFrame(int selected_frame);

private:

    void initLoggers();

private:

    Ui::MainWindow* _ui;

    NSXMenu *_menu_bar;

    QStatusBar *_status_bar;

    SessionModel *_session_model;

    TaskManagerModel *_task_manager_model;

    std::string _color_map;
};
