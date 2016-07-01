#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <unordered_map>
#include <vector>
#include <memory>

#include <QGraphicsScene>
#include <QMainWindow>
#include <QMenu>
#include <QProgressBar>

#include "IData.h"
#include "DialogPeakFind.h"
#include "Experiment.h"
#include "Logger.h"

#include "JobHandler.h"
#include "ProgressView.h"

class QListWidgetItem;
class PlottableGraphicsItem;

namespace Ui
{
    class MainWindow;
}

namespace SX
{
    namespace Data
    {
        class IData;
        class PeakFinder;
    }
    namespace Instrument
    {
        class Experiment;
    }

    namespace Utils {
        class ProgressHandler;
    }
}

class QProgressDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    Ui::MainWindow* getUI() const;

    ~MainWindow();

    void findPeaks();

signals:
    void plotDetectorData(SX::Data::IData*,int frame);
private slots:
//    void on_action_open_triggered();


    void on_actionPixel_position_triggered();
    void on_actionGamma_Nu_triggered();
    void on_action2_Theta_triggered();
    void on_actionD_spacing_triggered();
    void on_actionLogger_triggered();
    void on_action1D_Peak_Ploter_triggered();
    void updatePlot(PlottableGraphicsItem* cutter);
    void on_actionFrom_Sample_triggered();
    void on_actionBehind_Detector_triggered();
    void on_actionShow_labels_triggered(bool checked);
    void on_action_display_isotopes_database_triggered();
    void on_action_add_element_triggered();
    void on_actionConvert_to_HDF5_triggered();
    void on_actionH_k_l_triggered();
    void on_actionShow_calculated_peak_positions_triggered(bool checked);
    void on_checkBox_AspectRatio_toggled(bool checked);
    void on_actionConvolution_Filter_triggered();

public slots:
    void changeData(std::shared_ptr<SX::Data::IData>);
    void showPeakList(std::vector<std::shared_ptr<SX::Data::IData>>);
    void plotPeak(SX::Crystal::Peak3D*);
    void plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&);
    void setInspectorWidget(QWidget*);
private:
    Ui::MainWindow* _ui;
    std::unordered_map<std::string,SX::Instrument::Experiment> _experiments;
    std::shared_ptr<SX::Data::IData> _currentData;

    std::shared_ptr<SX::Utils::ProgressHandler> _progressHandler;
    std::shared_ptr<SX::Data::PeakFinder> _peakFinder;
    std::shared_ptr<SX::Imaging::Convolver> _convolver;
    //threshold, confidence, minComp, maxComp
};

#endif // MAINWINDOW_H
