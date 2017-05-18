#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <memory>

#include <QGraphicsScene>
#include <QMainWindow>
#include <QMenu>
#include <QProgressBar>

#include <nsxlib/data/IData.h>
#include <nsxlib/instrument/Experiment.h>
#include <nsxlib/imaging/Convolver.h>

#include "JobHandler.h"
#include "Logger.h"
#include "views/ProgressView.h"

class QListWidgetItem;
class PlottableGraphicsItem;
class SessionModel;

namespace Ui {
    class MainWindow;
}

namespace nsx {
    class Peak3D;
    using sptrPeak3D=std::shared_ptr<Peak3D>;
    class DataSet;
    class PeakFinder;
    class Experiment;
    class ProgressHandler;
}

class QProgressDialog;

using nsx::sptrPeak3D;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    Ui::MainWindow* getUI() const;

    ~MainWindow();

    void findPeaks();

signals:
    void plotDetectorData(nsx::DataSet*,int frame);
    void findSpaceGroup();
    void computeRFactors();
    void findFriedelPairs();
    //void integrateCalculatedPeaks();
    void peakFitDialog();
    void incorporateCalculatedPeaks();

private slots:
    void on_actionNew_session_triggered();
    void on_actionSave_session_triggered();
    void on_actionSave_session_as_triggered();
    void on_actionLoad_session_triggered();

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
    void on_actionShow_calculated_peak_positions_triggered(bool checked);
    void on_checkBox_AspectRatio_toggled(bool checked);

    void on_actionFind_space_group_triggered();
    void on_actionFind_Friedel_pairs_triggered();
    void on_actionCompute_R_factors_triggered();
    void on_actionIntegrate_calculated_peaks_triggered();
    void on_actionPeak_fit_dialog_triggered();

    void on_actionLogarithmic_Scale_triggered(bool checked);

    void on_actionDraw_peak_integration_area_triggered(bool checked);
    void on_actionRemove_bad_peaks_triggered(bool checked);
    void on_actionIncorporate_calculated_peaks_triggered(bool checked);
    void on_actionApply_resolution_cutoff_triggered();

    void on_actionWrite_log_file_triggered();
    void on_actionReintegrate_peaks_triggered();
    void on_actionFit_peak_profiles_triggered();
    void on_actionAuto_assign_unit_cell_triggered();

public slots:
    void changeData(std::shared_ptr<nsx::DataSet>);
    void plotPeak(sptrPeak3D);
    void plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&);
    void setInspectorWidget(QWidget*);
    void saveSession(QString filename);

private:
    Ui::MainWindow* _ui;
    std::shared_ptr<nsx::DataSet> _currentData;

    std::shared_ptr<nsx::ProgressHandler> _progressHandler;
    std::shared_ptr<nsx::PeakFinder> _peakFinder;
    std::shared_ptr<nsx::Convolver> _convolver;

    std::shared_ptr<SessionModel> _session;
};

#endif // MAINWINDOW_H
