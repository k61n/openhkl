#include <memory>

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QItemDelegate>
#include <QLineEdit>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>

#include <nsxlib/ConvolverFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/ITask.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/PixelSumIntegrator.h>

#include "CollectedPeaksModel.h"
#include "DoubleItemDelegate.h"
#include "ExperimentItem.h"
#include "FramePeakFinder.h"
#include "MainWindow.h"
#include "MetaTypes.h"
#include "PeakListItem.h"
#include "PeaksItem.h"
#include "PeakTableView.h"
#include "WidgetFoundPeaks.h"

#include "ui_FramePeakFinder.h"

FramePeakFinder* FramePeakFinder::_instance = nullptr;

FramePeakFinder* FramePeakFinder::create(MainWindow *main_window, ExperimentItem *experiment_item, const nsx::DataList& data)
{
    if (!_instance) {
        _instance = new FramePeakFinder(main_window, experiment_item, data);
    }

    return _instance;
}

FramePeakFinder* FramePeakFinder::Instance()
{
    return _instance;
}

FramePeakFinder::FramePeakFinder(MainWindow *main_window, ExperimentItem *experiment_item, const nsx::DataList& data)
: NSXQFrame(),
  _ui(new Ui::FramePeakFinder),
  _main_window(main_window),
  _experiment_item(experiment_item),
  _pixmap(nullptr),
  _colormap(new ColorMap),
  _peak_finders()
{
    _ui->setupUi(this);

    _ui->tabs->tabBar()->tabButton(0,QTabBar::RightSide)->hide();

    DoubleItemDelegate* convolution_parameters_delegate = new DoubleItemDelegate();
    _ui->convolution_parameters->setItemDelegateForColumn(1,convolution_parameters_delegate);

    for (auto d : data) {
        QFileInfo fileinfo(QString::fromStdString(d->filename()));
        _ui->selected_data->addItem(fileinfo.baseName(),QVariant::fromValue(d));
    }

    _ui->selected_data->setCurrentIndex(0);

    _ui->convolution_kernels->clear();
    nsx::ConvolverFactory convolver_factory;
    for (auto&& convolution_kernel : convolver_factory.callbacks()) {
        _ui->convolution_kernels->addItem(QString::fromStdString(convolution_kernel.first));
    }
    _ui->convolution_kernels->setCurrentText("annular");

    QGraphicsScene* scene = new QGraphicsScene();
    _ui->preview->setScene(scene);
    // flip the image vertically to conform with DetectorScene
    _ui->preview->scale(1, -1);

    updateConvolutionParameters();

    _ui->tabs->setCurrentIndex(0);

    // The min and max blob size update
    connect(_ui->min_blob_size,SIGNAL(valueChanged(int)),this,SLOT(changeMinBlobSize()));
    connect(_ui->max_blob_size,SIGNAL(valueChanged(int)),this,SLOT(changeMaxBlobSize()));

    // The integration parameters update
    connect(_ui->peak_scale,SIGNAL(valueChanged(double)),this,SLOT(changePeakScale()));
    connect(_ui->background_begin_scale,SIGNAL(valueChanged(double)),this,SLOT(changeBackgroundBegin()));
    connect(_ui->background_end_scale,SIGNAL(valueChanged(double)),this,SLOT(changeBackgroundEnd()));

    connect(_ui->convolution_kernels,SIGNAL(currentTextChanged(QString)),this,SLOT(changeConvolutionKernel(QString)));

    // The change of data to preview
    connect(_ui->selected_data,SIGNAL(currentIndexChanged(int)),this,SLOT(changePreviewData()));

    connect(_ui->apply_threshold,SIGNAL(stateChanged(int)),this,SLOT(clipPreview(int)));

    connect(_ui->selected_frame,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));
    connect(_ui->selected_frame_slider,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));

    connect(_ui->tabs->tabBar(),SIGNAL(tabBarDoubleClicked(int)),this,SLOT(slotTabEdited(int)));
    connect(_ui->tabs,SIGNAL(tabCloseRequested(int)),this,SLOT(slotTabRemoved(int)));

    connect(_ui->actions,SIGNAL(clicked(QAbstractButton*)),this,SLOT(doActions(QAbstractButton*)));

    connect(_main_window->taskManagerModel(),SIGNAL(sendCompletedTask(std::shared_ptr<nsx::ITask>)),this,SLOT(onShowFoundPeaks(std::shared_ptr<nsx::ITask>)));

    emit _ui->selected_data->currentIndexChanged(_ui->selected_data->currentIndex());
}

FramePeakFinder::~FramePeakFinder()
{
    delete _ui;

    disconnect(_main_window->taskManagerModel(),SIGNAL(sendCompletedTask(std::shared_ptr<nsx::ITask>)),this,SLOT(onShowFoundPeaks(std::shared_ptr<nsx::ITask>)));

    if (_instance) {
        _instance = nullptr;
    }
}

void FramePeakFinder::slotTabRemoved(int index)
{
    auto tab = dynamic_cast<WidgetFoundPeaks*>(_ui->tabs->widget(index));
    if (!tab) {
        return;
    }

    _ui->tabs->removeTab(index);

    delete tab;
}

void FramePeakFinder::slotTabEdited(int index)
{
    auto found_peaks_tab = dynamic_cast<WidgetFoundPeaks*>(_ui->tabs->widget(index));

    if (!found_peaks_tab) {
        return;
    }

    QInputDialog dialog(this);
    dialog.setLabelText("");
    dialog.setWindowTitle(tr("Set peak collection name"));
    auto pos = mapToGlobal(_ui->tabs->pos());

    int width(0);
    for (auto i = 0; i < index; ++i) {
        width += _ui->tabs->tabBar()->tabRect(index).width();
    }

    int height = _ui->tabs->tabBar()->tabRect(index).height();

    dialog.move(pos.x() + width,pos.y() + height);

    if (dialog.exec() == QDialog::Rejected) {
        return;
    }

    QString tab_name = dialog.textValue();
    if (tab_name.isEmpty()) {
        return;
    }

    _ui->tabs->setTabText(index,tab_name);
}

void FramePeakFinder::changeSelectedFrame(int selected_frame)
{
    _ui->selected_frame->setValue(selected_frame);
    _ui->selected_frame_slider->setValue(selected_frame);

    preview();
}

void FramePeakFinder::accept()
{    
    auto peaks_item = _experiment_item->peaksItem();

    for (auto i = 0; i < _ui->tabs->count(); ++i) {

        auto widget_found_peaks = dynamic_cast<WidgetFoundPeaks*>(_ui->tabs->widget(i));
        if (!widget_found_peaks) {
            continue;
        }

        auto&& found_peaks = widget_found_peaks->selectedPeaks();

        if (found_peaks.empty()) {
            continue;
        }

        auto checkbox = dynamic_cast<QCheckBox*>(_ui->tabs->tabBar()->tabButton(i,QTabBar::LeftSide));

        if (!checkbox->isChecked()) {
            continue;
        }

        auto item = new PeakListItem(found_peaks);

        item->setText(_ui->tabs->tabText(i));
        peaks_item->appendRow(item);
    }

    close();
}

void FramePeakFinder::setColorMap(const std::string &name)
{
    _colormap = std::unique_ptr<ColorMap>(new ColorMap(name));
}

void FramePeakFinder::run()
{
    nsx::info() << "Peak find algorithm: searching peaks in " << _ui->selected_data->count() << " files";

    nsx::DataList data;
    for (int i = 0; i < _ui->selected_data->count(); ++i) {
        data.push_back(_ui->selected_data->itemData(i,Qt::UserRole).value<nsx::sptrDataSet>());
    }

    std::shared_ptr<nsx::PeakFinder> peak_finder(new nsx::PeakFinder(data));

    _peak_finders.insert(peak_finder);

    peak_finder->setMinSize(_ui->min_blob_size->value());
    peak_finder->setMaxSize(_ui->max_blob_size->value());
    peak_finder->setMaxFrames(_ui->max_blob_width->value());

    // Get the current convolver type
    std::string convolver_type = _ui->convolution_kernels->currentText().toStdString();

    // Get the corresponding parameters
    auto&& parameters = convolutionParameters();

    nsx::ConvolverFactory convolver_factory;
    auto convolver = convolver_factory.create(convolver_type,parameters);

    // Propagate changes to peak finder
    peak_finder->setConvolver(std::unique_ptr<nsx::Convolver>(convolver));

    auto *task_manager_model = _main_window->taskManagerModel();

    task_manager_model->addTask(peak_finder,true);
}

void FramePeakFinder::onShowFoundPeaks(std::shared_ptr<nsx::ITask> task)
{
    showFoundPeaks(task);
}

void FramePeakFinder::showFoundPeaks(std::shared_ptr<nsx::ITask> task)
{
    auto peak_finder = std::dynamic_pointer_cast<nsx::PeakFinder>(task);

    auto it = _peak_finders.find(peak_finder);
    if (it == _peak_finders.end()) {
        return;
    }

    const auto &peaks = peak_finder->peaks();

    auto &&peak_end = _ui->peak_scale->value();
    auto &&background_begin = _ui->background_begin_scale->value();
    auto &&background_end = _ui->background_end_scale->value();

    // integrate peaks
    for (int i = 0; i < _ui->selected_data->count(); ++i) {
        auto dataset = _ui->selected_data->itemData(i,Qt::UserRole).value<nsx::sptrDataSet>();
        nsx::PixelSumIntegrator integrator(false,false);
        integrator.integrate(peaks, dataset,peak_end,background_begin,background_end);
    }

    WidgetFoundPeaks *widget_found_peaks = new WidgetFoundPeaks(_experiment_item,peaks);
    _ui->tabs->addTab(widget_found_peaks,"peaks");
    QCheckBox *checkbox = new QCheckBox();
    checkbox->setChecked(true);
    _ui->tabs->tabBar()->setTabButton(_ui->tabs->count()-1,QTabBar::LeftSide,checkbox);

    nsx::info() << "Peak search complete. Found " << peaks.size() << " peaks.";
}

std::map<std::string,double> FramePeakFinder::convolutionParameters() const
{
    std::map<std::string,double> parameters;

    for (int i = 0; i < _ui->convolution_parameters->rowCount(); ++i) {
        std::string pname = _ui->convolution_parameters->item(i,0)->text().toStdString();
        double pvalue = _ui->convolution_parameters->item(i,1)->text().toDouble();
        parameters.insert(std::make_pair(pname,pvalue));
    }

    return parameters;
}

void FramePeakFinder::changeConvolutionKernel(QString convolution_kernel)
{
    Q_UNUSED(convolution_kernel)

    // Update dialog with the selected convolver parameters
    updateConvolutionParameters();

    // Update the preview
    preview();
}

void FramePeakFinder::changeConvolutionParameters(int row, int col)
{
    Q_UNUSED(row)
    Q_UNUSED(col)

    // Update the preview
    preview();
}

void FramePeakFinder::updateConvolutionParameters()
{
    auto convolution_kernel_name = _ui->convolution_kernels->currentText().toStdString();

    nsx::ConvolverFactory convolution_kernel_factory;

    auto convolution_kernel = convolution_kernel_factory.create(convolution_kernel_name,{});

    // Get its corresponding parameters
    const std::map<std::string, double>& parameters = convolution_kernel->parameters();

    disconnect(_ui->convolution_parameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeConvolutionParameters(int,int)));

    _ui->convolution_parameters->setRowCount(0);

    // Iterate through parameters to build the tree
    for (auto p : parameters) {
        int current_row = _ui->convolution_parameters->rowCount();

        _ui->convolution_parameters->insertRow(current_row);

        QTableWidgetItem* pname = new QTableWidgetItem();
        pname->setText(QString::fromStdString(p.first));
        _ui->convolution_parameters->setItem(current_row,0,pname);

        pname->setFlags(pname->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem* pvalue = new QTableWidgetItem();
        pvalue->setText(QString::number(p.second));
        _ui->convolution_parameters->setItem(current_row,1,pvalue);
    }

    _ui->convolution_parameters->horizontalHeader()->setStretchLastSection(true);

    connect(_ui->convolution_parameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeConvolutionParameters(int,int)));
}

void FramePeakFinder::changePreviewData()
{
    auto data = _ui->selected_data->currentData(Qt::UserRole).value<nsx::sptrDataSet>();

    _ui->selected_frame->setMinimum(0);
    _ui->selected_frame->setMaximum(data->nFrames()-1);
    _ui->selected_frame->setValue(0);

    _ui->selected_frame_slider->setMinimum(0);
    _ui->selected_frame_slider->setMaximum(data->nFrames()-1);
    _ui->selected_frame_slider->setValue(0);

    preview();
}

void FramePeakFinder::preview()
{
    auto data = _ui->selected_data->currentData(Qt::UserRole).value<nsx::sptrDataSet>();

    int selected_frame = _ui->selected_frame->value();

    int nrows = data->nRows();
    int ncols = data->nCols();

    std::string convolver_type = _ui->convolution_kernels->currentText().toStdString();
    auto&& convolver_parameters = convolutionParameters();

    Eigen::MatrixXd convolved_frame = data->convolvedFrame(selected_frame,convolver_type, convolver_parameters);

    // apply threshold in preview
    if (_ui->apply_threshold->isChecked()) {
        double threshold_value = _ui->threshold->value();

        for (int i = 0; i < nrows; ++i) {
            for (int j = 0; j < ncols; ++j) {
                convolved_frame(i, j) = convolved_frame(i, j) < threshold_value ? 0 : 1;
            }
        }
    }

    // clamp the result for the preview window
    double minVal = convolved_frame.minCoeff();
    double maxVal = convolved_frame.maxCoeff();

    // avoid division by zero
    if (maxVal-minVal <= 0.0) {
        maxVal = minVal + 1.0;
    }
    convolved_frame.array() -= minVal;
    convolved_frame.array() /= maxVal-minVal;

    QRect rect(0, 0, ncols, nrows);
    QImage image = _colormap->matToImage(convolved_frame.cast<double>(), rect, convolved_frame.maxCoeff());

    if (!_pixmap) {
        _pixmap = _ui->preview->scene()->addPixmap(QPixmap::fromImage(image));
    } else {
        _pixmap->setPixmap(QPixmap::fromImage(image));
    }

    _ui->preview->fitInView(_ui->preview->scene()->sceneRect());
}

void FramePeakFinder::clipPreview(int state) {
    Q_UNUSED(state)

    preview();
}

void FramePeakFinder::showEvent(QShowEvent* event)
{
    Q_UNUSED(event)

    _ui->preview->fitInView(_ui->preview->scene()->sceneRect());
}

void FramePeakFinder::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)

    _ui->preview->fitInView(_ui->preview->scene()->sceneRect());

    NSXQFrame::resizeEvent(event);
}

void FramePeakFinder::changeMinBlobSize()
{
    auto&& value = _ui->min_blob_size->value();

    _ui->min_blob_size->setValue(std::min(value,_ui->max_blob_size->value()));
}

void FramePeakFinder::changeMaxBlobSize()
{
    auto&& value = _ui->max_blob_size->value();

    _ui->max_blob_size->setValue(std::max(value,_ui->min_blob_size->value()));
}

void FramePeakFinder::changePeakScale()
{
    auto&& value = _ui->peak_scale->value();

    _ui->peak_scale->setValue(std::min(value,_ui->background_begin_scale->value()));
}

void FramePeakFinder::changeBackgroundBegin()
{
    auto&& value = _ui->background_begin_scale->value();

    value = std::max(value,_ui->peak_scale->value());

    _ui->background_begin_scale->setValue(std::min(value,_ui->background_end_scale->value()));
}

void FramePeakFinder::changeBackgroundEnd()
{
    auto&& value = _ui->background_end_scale->value();

    _ui->background_end_scale->setValue(std::max(value,_ui->background_begin_scale->value()));
}

void FramePeakFinder::doActions(QAbstractButton *button)
{
    auto button_role = _ui->actions->standardButton(button);

    switch(button_role)
    {
    case QDialogButtonBox::StandardButton::Apply: {
        run();
        break;
    }
    case QDialogButtonBox::StandardButton::Cancel: {
        close();
        break;
    }
    case QDialogButtonBox::StandardButton::Ok: {
        accept();
        break;
    }
    default: {
        return;
    }
    }
}
