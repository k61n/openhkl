#include <cmath>
#include <functional>
#include <stdexcept>

#include <Eigen/Dense>

#include <QDateTime>
#include <QDebug>
#include <QDockWidget>
#include <QFileDialog>
#include <QGraphicsBlurEffect>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QProgressDialog>
#include <QShortcut>
#include <QThread>
#include <QTransform>

#include <nsxlib/crystal/GruberReduction.h>
#include <nsxlib/crystal/NiggliReduction.h>
#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/crystal/SpaceGroup.h>
#include <nsxlib/crystal/SpaceGroupSymbols.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/data/IFrameIterator.h>
#include <nsxlib/data/PeakFinder.h>
#include <nsxlib/geometry/AABB.h>
#include <nsxlib/geometry/Basis.h>
#include <nsxlib/geometry/BlobFinder.h>
#include <nsxlib/geometry/Ellipsoid.h>
#include <nsxlib/instrument/ComponentState.h>
#include <nsxlib/instrument/Detector.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/instrument/Source.h>
#include <nsxlib/mathematics/MathematicsTypes.h>
#include <nsxlib/utils/ProgressHandler.h>
#include <nsxlib/utils/Path.h>
#include <nsxlib/utils/Units.h>

#include "absorption/AbsorptionWidget.h"
#include "dialogs/IsotopeDatabaseDialog.h"
#include "DetectorScene.h"
#include "ColorMap.h"
#include "dialogs/DialogConvolve.h"
#include "dialogs/DialogExperiment.h"
#include "dialogs/DialogIntegrate.h"
#include "dialogs/NumorsConversionDialog.h"
#include "dialogs/ResolutionCutoffDialog.h"
#include "items/CutLineGraphicsItem.h"
#include "items/CutSliceGraphicsItem.h"
#include "items/CutterGraphicsItem.h"
#include "items/PeakGraphicsItem.h"
#include "items/PlottableGraphicsItem.h"
#include "JobHandler.h"
#include "Logger.h"
#include "models/CollectedPeaksModel.h"
#include "models/SessionModel.h"
#include "NoteBook.h"
#include "plot/PlotFactory.h"
#include "plot/SXPlot.h"
#include "tree/ExperimentTree.h"
#include "tree/PeakListPropertyWidget.h"
#include "tree/UnitCellPropertyWidget.h"
#include "views/PeakTableView.h"
#include "views/PeakTableView.h"

#include "MainWindow.h"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent),
  _ui(new Ui::MainWindow),
  //_experiments(),
  _currentData(nullptr),
  _session(new SessionModel)
{
    _ui->setupUi(this);

    // make experiment tree aware of the session
    _ui->experimentTree->setSession(_session);

    // Set Date to the application window title
    QDateTime datetime=QDateTime::currentDateTime();
    this->setWindowTitle(QString("NSXTool version:")+ datetime.toString());

    // Starting the logger of the main application
    Logger::Instance()->setNoteBook(_ui->noteBook);
    qInstallMessageHandler(customMessageHandler);
    qDebug() << "[NSXTool log]" << QDateTime::currentDateTime().toString();

    //
    _ui->frameFrame->setEnabled(false);
    _ui->intensityFrame->setEnabled(false);

    _ui->selectionMode->addItem(QIcon(":/resources/zoomIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/cutlineIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/horizontalSliceIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/verticalSliceIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/selectionIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/slice3D.png"),"");

    // Vertical splitter between Tree and Inspector Widget
    _ui->splitterVertical->setStretchFactor(0,50);
    _ui->splitterVertical->setStretchFactor(1,50);

    // Horizontal splitter between Tree and DetectorScene
    _ui->splitterHorizontal->setStretchFactor(0,10);
    _ui->splitterHorizontal->setStretchFactor(1,90);

    // signals and slots
    connect(_ui->experimentTree, SIGNAL(plotData(nsx::sptrDataSet)),
            _ui->_dview->getScene(), SLOT(setData(nsx::sptrDataSet))
    );

    connect(_ui->experimentTree, SIGNAL(plotData(nsx::sptrDataSet)),
            this, SLOT(changeData(nsx::sptrDataSet)));

    connect(_ui->frame,&QScrollBar::valueChanged,[=](const int& value){_ui->_dview->getScene()->changeFrame(value);});

    connect(_ui->intensity,SIGNAL(valueChanged(int)),_ui->_dview->getScene(),SLOT(setMaxIntensity(int)));
    connect(_ui->selectionMode,SIGNAL(currentIndexChanged(int)),_ui->_dview->getScene(),SLOT(changeInteractionMode(int)));
    connect(_ui->_dview->getScene(),SIGNAL(updatePlot(PlottableGraphicsItem*)),this,SLOT(updatePlot(PlottableGraphicsItem*)));
    connect(_ui->action_open,SIGNAL(triggered()),_ui->experimentTree,SLOT(createNewExperiment()));

    connect(_ui->experimentTree, SIGNAL(resetScene()),
            _ui->_dview->getScene(), SLOT(resetScene()));

    connect(this, SIGNAL(findSpaceGroup(void)), _ui->experimentTree, SLOT(findSpaceGroup()));
    connect(this, SIGNAL(computeRFactors(void)), _ui->experimentTree, SLOT(computeRFactors()));
    connect(this,SIGNAL(findFriedelPairs(void)), _ui->experimentTree, SLOT(findFriedelPairs()));
    // connect(this, SIGNAL(integrateCalculatedPeaks()), _ui->experimentTree, SLOT(integrateCalculatedPeaks()));
    connect(this, SIGNAL(peakFitDialog()), _ui->experimentTree, SLOT(peakFitDialog()));
    connect(this, SIGNAL(incorporateCalculatedPeaks()), _ui->experimentTree, SLOT(incorporateCalculatedPeaks()));

    connect(_session.get(), SIGNAL(updatePeaks()), _ui->_dview->getScene(), SLOT(updatePeaks()));

    _ui->loggerDockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    _ui->plotterDockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    _ui->dockWidget_Property->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);

    _ui->plotterDockWidget->show();
    _ui->dockWidget_Property->show();

    connect(_ui->experimentTree,SIGNAL(inspectWidget(QWidget*)),this,SLOT(setInspectorWidget(QWidget*)));

    qDebug() << "The resources directory is " << nsx::Path().getResourcesDir().c_str();

    _progressHandler = nsx::sptrProgressHandler(new nsx::ProgressHandler());
    _peakFinder = nsx::sptrPeakFinder(new nsx::PeakFinder());



    for (auto&& action: _ui->menuColor_map->actions()) {
        _ui->menuColor_map->removeAction(action);
    }

    auto names = ColorMap::getColorMapNames();

    for (auto&& name: names) {
        QAction* action = new QAction(name.c_str(), _ui->menuColor_map);

        auto slot_fn = [=] () -> void
        {
            const std::string name_str = action->text().toStdString();
            _session->setColorMap(name_str);
            _ui->_dview->getScene()->setColorMap(name_str);
            _ui->_dview->getScene()->redrawImage();
        };

        connect(action, &QAction::triggered, this, slot_fn);
        _ui->menuColor_map->addAction(action);
    }
}

MainWindow::~MainWindow()
{
    qInstallMessageHandler(0);
    delete _ui;
}

void MainWindow::on_actionNew_session_triggered()
{
  qDebug() << "save session: not implemented yet";
}

void MainWindow::saveSession(QString filename)
{
    if (filename == "") {
        qDebug() << "Must first save to file before selecting save";
        return;
    }

    qDebug() << "saving session to " << filename;

    QJsonDocument doc(_session->toJsonObject());

    QFile savefile(filename);

    if ( !savefile.open(QIODevice::WriteOnly)) {
        qDebug() << "couldn't open file for saving!";
        return;
    }

    savefile.write(doc.toJson());
    _session->setFilename(filename);
}

void MainWindow::on_actionSave_session_triggered()
{
    saveSession(_session->getFilename());
}

void MainWindow::on_actionSave_session_as_triggered()
{
    QString homeDir = nsx::Path::getHomeDirectory().c_str();
    QString filename = QFileDialog::getSaveFileName(this, "Save session as..", homeDir, "Json document (*.json)", nullptr, QFileDialog::Option::DontUseNativeDialog);
    saveSession(filename);
}

void MainWindow::on_actionLoad_session_triggered()
{
    QString homeDir = nsx::Path::getHomeDirectory().c_str();
    QString filename = QFileDialog::getOpenFileName(this, "Load session", homeDir, "Json document (*.json)", nullptr, QFileDialog::Option::DontUseNativeDialog);
    qDebug() << "Loading session from file '" << filename << "'";

    QFile loadfile(filename);

    if ( !loadfile.open(QIODevice::ReadOnly)) {
        qDebug() << "couldn't open file for loading!";
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(loadfile.readAll());
    QJsonObject obj = doc.object();

    _session->fromJsonObject(obj);
    _session->setFilename(filename);
}

void MainWindow::on_actionAbout_triggered()
{
    qDebug() << "about triggered";
    // Show splash
    QImage splashScrImage(":/resources/splashScreen.png");
    QPixmap Logo;
    Logo.convertFromImage(splashScrImage);
    QSplashScreen* splashScrWindow = new QSplashScreen(this, Logo, Qt::WindowStaysOnTopHint);
    QSize screenSize = QApplication::desktop()->geometry().size();
    splashScrWindow->move(screenSize.width()/2-300,screenSize.height()/2-150);
    splashScrWindow->show();
}

Ui::MainWindow* MainWindow::getUI() const
{
    return _ui;
}


void MainWindow::changeData(nsx::sptrDataSet data)
{
    _ui->frameFrame->setEnabled(true);
    _ui->intensityFrame->setEnabled(true);

    int frameMax = int(data->getNFrames()-1);
    int frame = _ui->frame->value();

    if (frame > frameMax)
        frame = frameMax;

    _ui->frame->setValue(frame);
    _ui->frame->setMaximum(frameMax);
    _ui->spinBox_Frame->setMaximum(frameMax);

    //_ui->intensity->setValue(10);
}

void MainWindow::plotPeak(nsx::sptrPeak3D peak)
{
    auto scenePtr = _ui->_dview->getScene();
    // Ensure that frames
    changeData(peak->getData());
    // Get frame number to adjust the data
    size_t data_frame = size_t(std::lround(peak->getShape().getAABBCenter()[2]));
    scenePtr->setData(peak->getData(), data_frame);
    // Update the scrollbar
    _ui->frame->setValue(data_frame);
    auto pgi = scenePtr->findPeakGraphicsItem(peak);

    if (pgi)
        updatePlot(pgi);
}

void MainWindow::on_actionPixel_position_triggered()
{
   _ui->_dview->getScene()->changeCursorMode(DetectorScene::PIXEL);
}

void MainWindow::on_actionGamma_Nu_triggered()
{
    _ui->_dview->getScene()->changeCursorMode(DetectorScene::GAMMA);
}

void MainWindow::on_action2_Theta_triggered()
{
     _ui->_dview->getScene()->changeCursorMode(DetectorScene::THETA);
}

void MainWindow::on_actionH_k_l_triggered()
{
     _ui->_dview->getScene()->changeCursorMode(DetectorScene::HKL);
}


void MainWindow::on_actionD_spacing_triggered()
{
  _ui->_dview->getScene()->changeCursorMode(DetectorScene::DSPACING);
}

void MainWindow::on_actionLogger_triggered()
{
    if (_ui->loggerDockWidget->isHidden())
        _ui->loggerDockWidget->show();
    else
        _ui->loggerDockWidget->hide();
}

void MainWindow::on_action1D_Peak_Ploter_triggered()
{
    if (_ui->plotterDockWidget->isHidden())
        _ui->plotterDockWidget->show();
    else
        _ui->plotterDockWidget->hide();
}

void MainWindow::on_actionProperty_triggered()
{
    if (_ui->dockWidget_Property->isHidden())
        _ui->dockWidget_Property->show();
    else
        _ui->dockWidget_Property->hide();
}

void MainWindow::plotData(const QVector<double>& x,const QVector<double>& y,const QVector<double>& e)
{
    if (_ui->plot1D->getType().compare("simple") != 0) {
        // Store the old size policy
        QSizePolicy oldSizePolicy = _ui->plot1D->sizePolicy();
        // Remove the current plotter from the ui
        _ui->horizontalLayout_4->removeWidget(_ui->plot1D);
        // Delete the plotter instance
        delete _ui->plot1D;

        PlotFactory* pFactory=PlotFactory::Instance();

        _ui->plot1D = pFactory->create("simple",_ui->dockWidgetContents_4);

        // Restore the size policy
        _ui->plot1D->setSizePolicy(oldSizePolicy);

        // Sets some properties of the plotter
        _ui->plot1D->setObjectName(QStringLiteral("1D plotter"));
        _ui->plot1D->setFocusPolicy(Qt::StrongFocus);
        _ui->plot1D->setStyleSheet(QStringLiteral(""));

        // Add the plot to the ui
        _ui->horizontalLayout_4->addWidget(_ui->plot1D);
    }

    _ui->plot1D->graph(0)->setDataValueError(x,y,e);
    _ui->plot1D->rescaleAxes();
    _ui->plot1D->replot();
}

void MainWindow::updatePlot(PlottableGraphicsItem* item)
{
    if (!item)
        return;

    if (!item->isPlottable(_ui->plot1D)) {
        // Store the old size policy
        QSizePolicy oldSizePolicy = _ui->plot1D->sizePolicy();
        // Remove the current plotter from the ui
        _ui->horizontalLayout_4->removeWidget(_ui->plot1D);
        // Delete the plotter instance
        delete _ui->plot1D;

        PlotFactory* pFactory=PlotFactory::Instance();

        _ui->plot1D = pFactory->create(item->getPlotType(),_ui->dockWidgetContents_4);

        // Restore the size policy
        _ui->plot1D->setSizePolicy(oldSizePolicy);

        // Sets some properties of the plotter
        _ui->plot1D->setObjectName(QStringLiteral("1D plotter"));
        _ui->plot1D->setFocusPolicy(Qt::StrongFocus);
        _ui->plot1D->setStyleSheet(QStringLiteral(""));

        // Add the plot to the ui
        _ui->horizontalLayout_4->addWidget(_ui->plot1D);
    }

    // Plot the data
    item->plot(_ui->plot1D);
    update();

}

void MainWindow::on_actionFrom_Sample_triggered()
{
    QTransform trans;
    trans.scale(1,-1);
    _ui->_dview->setTransform(trans);
    _ui->_dview->fitScene();
}

void MainWindow::on_actionBehind_Detector_triggered()
{
    QTransform trans;
    trans.scale(-1,-1);
    _ui->_dview->setTransform(trans);
    _ui->_dview->fitScene();
}

void MainWindow::on_action_display_isotopes_database_triggered()
{
    // Opens the dialog that will diplay the isotopes database
    IsotopeDatabaseDialog* dlg=new IsotopeDatabaseDialog();
    dlg->exec();
}

void MainWindow::on_actionShow_labels_triggered(bool checked)
{
    _ui->_dview->getScene()->showPeakLabels(checked);
    _ui->_dview->getScene()->update();
}


void MainWindow::setInspectorWidget(QWidget* w)
{
    // Ensure that previous Property Widget is deleted.
    auto widget=_ui->dockWidget_Property->widget();
    if (widget)
        delete widget;

    // Assign current property Widget
    _ui->dockWidget_Property->setWidget(w);

    if (PeakListPropertyWidget* widget=dynamic_cast<PeakListPropertyWidget*>(w)) {
        // Ensure plot1D is updated
        connect(widget->getPeakTableView(),SIGNAL(plotPeak(nsx::sptrPeak3D)),this,SLOT(plotPeak(nsx::sptrPeak3D)));
        connect(widget->getPeakTableView(),
                SIGNAL(plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&)),
                this,
                SLOT(plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&)));

        CollectedPeaksModel* peakModel = dynamic_cast<CollectedPeaksModel*>(widget->getPeakTableView()->model());
        connect(peakModel,SIGNAL(unitCellUpdated()),_ui->_dview->getScene(),SLOT(updatePeaks()));
    }
}

void MainWindow::on_actionConvert_to_HDF5_triggered()
{
    NumorsConversionDialog* dlg=new NumorsConversionDialog();
    dlg->exec();
}

void MainWindow::on_checkBox_AspectRatio_toggled(bool checked)
{
    _ui->_dview->fixDetectorAspectRatio(checked);
}

void MainWindow::on_actionFind_space_group_triggered()
{
    emit findSpaceGroup();
}

void MainWindow::on_actionFind_Friedel_pairs_triggered()
{
    emit findFriedelPairs();
}


void MainWindow::on_actionCompute_R_factors_triggered()
{
    emit computeRFactors();
    // emit findFriedelPairs();
}

void MainWindow::on_actionIntegrate_calculated_peaks_triggered()
{
//    emit integrateCalculatedPeaks();
    qDebug() << "what triggered this?";
}

void MainWindow::on_actionPeak_fit_dialog_triggered()
{
    emit peakFitDialog();
}

void MainWindow::on_actionLogarithmic_Scale_triggered(bool checked)
{
    _ui->_dview->getScene()->setLogarithmic(checked);
}

void MainWindow::on_actionDraw_peak_integration_area_triggered(bool checked)
{
    _ui->_dview->getScene()->drawIntegrationRegion(checked);
}

void MainWindow::on_actionRemove_bad_peaks_triggered(bool checked)
{
    //const double pmax = 2.873e-7; // corresponds to 5 sigma
    // const double pmax = 3e-5; // corresponds to 4 sigma
    const double pmax = 1e-3;
    int total_peaks = 0;
    // int remaining_peaks = 0;

    nsx::DataList numors = _session->getSelectedNumors();
    nsx::PeakList bad_peaks;

    for (nsx::sptrDataSet numor: numors) {

        numor->removeDuplicatePeaks();

        nsx::PeakSet& peaks = numor->getPeaks();

        total_peaks += peaks.size();

        for (auto it = peaks.begin(); it != peaks.end();) {
            if ( (*it)->isMasked() || !(*it)->isSelected() ) {
                bad_peaks.push_back(*it);
                it = peaks.erase(it);
            }
            else if ((*it)->pValue() >= pmax) {
                bad_peaks.push_back(*it);
                it = peaks.erase(it);
            }
            else {
                bool correctly_indexed = false;

                for (int i = 0; i < numor->getDiffractometer()->getSample()->getNCrystals(); ++i) {
                    nsx::UnitCell cell = *numor->getDiffractometer()->getSample()->getUnitCell(i);
                    Eigen::RowVector3d hkl;
                    bool indexingSuccess = (*it)->getMillerIndices(cell,hkl,true);
                    if (indexingSuccess) {
                        correctly_indexed = true;
                        break;
                    }
                }
                if (!correctly_indexed) {
                    bad_peaks.push_back(*it);
                    it = peaks.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    qDebug() << "Eliminated " << bad_peaks.size() << " out of " << total_peaks << " total peaks.";
    //_ui->_dview->getScene()->updatePeaks();
    _session->updatePeaks();
}

void MainWindow::on_actionIncorporate_calculated_peaks_triggered(bool checked)
{
    emit incorporateCalculatedPeaks();
}

void MainWindow::on_actionApply_resolution_cutoff_triggered()
{
    ResolutionCutoffDialog dialog;

    if (!dialog.exec())
        return;

    _session->applyResolutionCutoff(dialog.dMin(), dialog.dMax());
}

void MainWindow::on_actionWrite_log_file_triggered()
{
    qDebug() << "write log file triggered";
    _session->writeLog();
}

void MainWindow::on_actionReintegrate_peaks_triggered()
{
    qDebug() << "Reintegrating peaks...";

    auto dialog = new DialogIntegrate();

    if (!dialog->exec()) {
        qDebug() << "Peak integration canceled.";
        return;
    }

    const double peak_scale = dialog->peakScale();
    const double bkg_scale = dialog->backgroundScale();
    const bool update_shape = dialog->updateShape();

    nsx::DataList numors = _session->getSelectedNumors();

    for (auto&& numor: numors) {
        numor->integratePeaks(peak_scale, bkg_scale, update_shape, _progressHandler);
    }

    _session->updatePeaks();
    qDebug() << "Done reintegrating peaks intensities";
}

void MainWindow::on_actionFit_peak_profiles_triggered()
{
    qDebug() << "Fit peak profiles triggered";
    _session->peakFitDialog();
}

void MainWindow::on_actionAuto_assign_unit_cell_triggered()
{
    qDebug() << "Auto assign unit cell triggered";
    _session->autoAssignUnitCell();
}
