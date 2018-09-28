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
#include <nsxlib/Logger.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/PixelSumIntegrator.h>

#include "CollectedPeaksModel.h"
#include "ExperimentItem.h"
#include "FramePeakFinder.h"
#include "MetaTypes.h"
#include "PeakListItem.h"
#include "PeaksItem.h"
#include "PeakTableView.h"
#include "ProgressView.h"

class DoubleDelegate : public QItemDelegate
{
public:
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem & option,const QModelIndex & index) const
    {
        Q_UNUSED(option)
        Q_UNUSED(index)

        QLineEdit* lineEdit = new QLineEdit(parent);
        // Set validator
        QDoubleValidator *validator = new QDoubleValidator(lineEdit);
        lineEdit->setValidator(validator);
        return lineEdit;
    }
};

FramePeakFinder* FramePeakFinder::_instance = nullptr;

FramePeakFinder* FramePeakFinder::create(ExperimentItem *experiment_item, const nsx::DataList& data)
{
    if (!_instance) {
        _instance = new FramePeakFinder(experiment_item, data);
    }

    return _instance;
}

FramePeakFinder* FramePeakFinder::Instance()
{
    return _instance;
}

FramePeakFinder::FramePeakFinder(ExperimentItem *experiment_item, const nsx::DataList& data)
: NSXQFrame(),
  _experiment_item(experiment_item),
  _pixmap(nullptr),
  _colormap(new ColorMap)
{
    setWindowTitle(tr("Peak finder"));

    setupLayout();

    for (auto d : data) {
        QFileInfo fileinfo(QString::fromStdString(d->filename()));
        _preview_data->addItem(fileinfo.baseName(),QVariant::fromValue(d));
    }

    _preview_data->setCurrentIndex(0);

    _convolution_kernels->clear();
    nsx::ConvolverFactory convolver_factory;
    for (auto&& convolution_kernel : convolver_factory.callbacks()) {
        _convolution_kernels->addItem(QString::fromStdString(convolution_kernel.first));
    }
    _convolution_kernels->setCurrentText("annular");

    updateConvolutionParameters();

    setupConnections();

    emit _preview_data->currentIndexChanged(_preview_data->currentIndex());
}

FramePeakFinder::~FramePeakFinder()
{
    if (_instance) {
        _instance = nullptr;
    }
}

void FramePeakFinder::setupLayout()
{

    // Set the tabs
    QTabWidget *tabs = new QTabWidget(this);
    tabs->addTab(new QWidget(),"Settings");
    tabs->addTab(new QWidget(),"Found peaks");

    // The threshold spinbox
    _threshold = new QSpinBox();
    _threshold->setMinimum(1);
    _threshold->setMaximum(1e6);
    _threshold->setValue(3);
    _threshold->setWrapping(true);
    _threshold->setSingleStep(1);
    _threshold->setToolTip("The threshold for considering a pixel being part of a peak");

    // The merge scale spinbox
    _merge_scale = new QDoubleSpinBox();
    _merge_scale->setMinimum(1.0);
    _merge_scale->setMaximum(10.0);
    _merge_scale->setValue(1.0);
    _merge_scale->setWrapping(true);
    _merge_scale->setSingleStep(1.0e-3);
    _merge_scale->setToolTip("The blob merging scale (in \u03C3 unit)");

    // The minimum blob size spinbox
    _min_blob_size = new QSpinBox();
    _min_blob_size->setMinimum(1);
    _min_blob_size->setMaximum(1e3);
    _min_blob_size->setValue(3e1);
    _min_blob_size->setWrapping(true);
    _min_blob_size->setSingleStep(1);
    _min_blob_size->setToolTip("The minimum size of a blob for being considered as a peak");

    // The maximum blob size spinbox
    _max_blob_size = new QSpinBox();
    _max_blob_size->setMinimum(1);
    _max_blob_size->setMaximum(1e4);
    _max_blob_size->setValue(1e3);
    _max_blob_size->setWrapping(true);
    _max_blob_size->setSingleStep(1);
    _max_blob_size->setToolTip("The maximum size of a blob for being considered as a peak");

    // The blob width spinbox
    _max_blob_width = new QSpinBox();
    _max_blob_width->setMinimum(1e1);
    _max_blob_width->setMaximum(1e2);
    _max_blob_width->setValue(1e1);
    _max_blob_width->setWrapping(true);
    _max_blob_width->setSingleStep(1);
    _max_blob_width->setToolTip("The maximum width in frames of a blob for being considered as a peak");

    _convolution_kernels = new QComboBox();
    _convolution_kernels->setToolTip("The convolution kernel used for smoothing the image prior finding peaks");

    _convolution_parameters = new QTableWidget();
    _convolution_parameters->setColumnCount(2);
    _convolution_parameters->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    _convolution_parameters->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _convolution_parameters->horizontalHeader()->hide();
    _convolution_parameters->verticalHeader()->hide();
    DoubleDelegate* convolution_parameters_delegate = new DoubleDelegate();
    _convolution_parameters->setItemDelegateForColumn(1,convolution_parameters_delegate);
    _convolution_parameters->setToolTip("The parameters of the convolution kernel used for smoothing the image prior finding peaks");

    // The Data list combox
    _preview_data = new QComboBox();
    _preview_data->setToolTip("The selected data for previewing the peak search");

    // The frame number spinbox
    _preview_frame_number = new QSpinBox();
    _preview_frame_number->setMinimum(0);
    _preview_frame_number->setMaximum(1e4);
    _preview_frame_number->setValue(0);
    _preview_frame_number->setWrapping(true);
    _preview_frame_number->setSingleStep(1);
    _preview_frame_number->setToolTip("The value of the selected frame for previewing the peak search");

    // The frame number slider
    _preview_frame = new QSlider();
    _preview_frame->setOrientation(Qt::Horizontal);
    _preview_frame->setMinimum(0);
    _preview_frame->setMaximum(1e4);
    _preview_frame->setValue(0);
    _preview_frame->setSingleStep(1);
    _preview_frame->setToolTip("The selected frame for previewing the peak search");

    // The apply theshold to preview check box
    _apply_threshold = new QCheckBox();
    _apply_threshold->setToolTip("Apply the thershold to the preview");

    // The peak area scale spinbox
    _peak_scale = new QDoubleSpinBox();
    _peak_scale->setDecimals(1);
    _peak_scale->setMinimum(1.0);
    _peak_scale->setMaximum(1.0e3);
    _peak_scale->setValue(3.0);
    _peak_scale->setWrapping(true);
    _peak_scale->setSingleStep(1.0e-1);
    _peak_scale->setToolTip("The peak area scale (in \u03C3 unit)");

    // The background lower limit area scale spinbox
    _background_begin_scale = new QDoubleSpinBox();
    _background_begin_scale->setDecimals(1);
    _background_begin_scale->setMinimum(1.0);
    _background_begin_scale->setMaximum(1.0e3);
    _background_begin_scale->setValue(4.0);
    _background_begin_scale->setWrapping(true);
    _background_begin_scale->setSingleStep(1.0e-1);
    _background_begin_scale->setToolTip("The background lower limit area scale (in \u03C3 unit)");

    // The background upper limit area scale spinbox
    _background_end_scale = new QDoubleSpinBox();
    _background_end_scale->setDecimals(1);
    _background_end_scale->setMinimum(1.0);
    _background_end_scale->setMaximum(1.0e3);
    _background_end_scale->setValue(4.5);
    _background_end_scale->setWrapping(true);
    _background_end_scale->setSingleStep(1.0e-1);
    _background_end_scale->setToolTip("The background upper limit area scale (in \u03C3 unit)");

    QGroupBox *blob_search_box = new QGroupBox(tr("Blob search parameters"));
    QGroupBox *preview_box = new QGroupBox(tr("Preview"));
    QGroupBox *integration_box = new QGroupBox(tr("Integration parameters"));

    _preview = new QGraphicsView();
    QGraphicsScene* scene = new QGraphicsScene();
    _preview->setScene(scene);
    // flip the image vertically to conform with DetectorScene
    _preview->scale(1, -1);

    _peaks = new PeakTableView();
    CollectedPeaksModel *peaks_model = new CollectedPeaksModel(_experiment_item->model(),_experiment_item->experiment(),{});
    _peaks->setModel(peaks_model);

    _actions = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok | QDialogButtonBox::Apply,this);

    // Set the layout
    QVBoxLayout *main_layout = new QVBoxLayout(this);

    QHBoxLayout *settings_tabs_layout = new QHBoxLayout();

    QVBoxLayout *parameters_layout = new QVBoxLayout();

    QFormLayout *blob_search_box_layout = new QFormLayout();
    blob_search_box_layout->addRow(new QLabel("threshold"),_threshold);
    blob_search_box_layout->addRow(new QLabel("merging scale"),_merge_scale);
    blob_search_box_layout->addRow(new QLabel("minimimum size"),_min_blob_size);
    blob_search_box_layout->addRow(new QLabel("maximum size"),_max_blob_size);
    blob_search_box_layout->addRow(new QLabel("maximum number of frames"),_max_blob_width);
    blob_search_box_layout->addRow(new QLabel("convolution kernel"),_convolution_kernels);
    blob_search_box_layout->addRow(new QLabel("convolution parameters"),_convolution_parameters);
    blob_search_box->setLayout(blob_search_box_layout);

    QFormLayout *preview_box_layout = new QFormLayout();
    preview_box_layout->addRow(new QLabel("data"),_preview_data);
    QHBoxLayout *preview_frame_layout = new QHBoxLayout();
    preview_frame_layout->addWidget(_preview_frame_number);
    preview_frame_layout->addWidget(_preview_frame);
    preview_box_layout->addRow(new QLabel("frame"),preview_frame_layout);
    preview_box_layout->addRow(new QLabel("apply threshold"),_apply_threshold);
    preview_box->setLayout(preview_box_layout);

    QFormLayout *integration_box_layout = new QFormLayout();
    integration_box_layout->addRow(new QLabel("peak area"),_peak_scale);
    integration_box_layout->addRow(new QLabel("background lower limit"),_background_begin_scale);
    integration_box_layout->addRow(new QLabel("background upper limit"),_background_end_scale);
    integration_box->setLayout(integration_box_layout);

    parameters_layout->addWidget(blob_search_box);
    parameters_layout->addWidget(preview_box);
    parameters_layout->addWidget(integration_box);
    parameters_layout->addStretch(1);

    settings_tabs_layout->addLayout(parameters_layout);
    settings_tabs_layout->addWidget(_preview);
    settings_tabs_layout->setStretch(0,2);
    settings_tabs_layout->setStretch(1,3);

    auto settings_tab = tabs->widget(0);
    settings_tab->setLayout(settings_tabs_layout);

    auto found_peaks_tab = tabs->widget(1);
    QHBoxLayout *found_peaks_tabs_layout = new QHBoxLayout();
    found_peaks_tabs_layout->addWidget(_peaks);
    found_peaks_tab->setLayout(found_peaks_tabs_layout);

    main_layout->addWidget(tabs);
    main_layout->addWidget(_actions);

    setLayout(main_layout);

    setMinimumSize(1000,800);

    tabs->setCurrentIndex(0);
}

void FramePeakFinder::setupConnections()
{
    // The min and max blob size update
    connect(_min_blob_size,SIGNAL(valueChanged(int)),this,SLOT(changeMinBlobSize()));
    connect(_max_blob_size,SIGNAL(valueChanged(int)),this,SLOT(changeMaxBlobSize()));

    // The integration parameters update
    connect(_peak_scale,SIGNAL(valueChanged(double)),this,SLOT(changePeakScale()));
    connect(_background_begin_scale,SIGNAL(valueChanged(double)),this,SLOT(changeBackgroundBegin()));
    connect(_background_end_scale,SIGNAL(valueChanged(double)),this,SLOT(changeBackgroundEnd()));

    connect(_convolution_kernels,SIGNAL(currentTextChanged(QString)),this,SLOT(changeConvolutionKernel(QString)));

    // The change of data to preview
    connect(_preview_data,SIGNAL(currentIndexChanged(int)),this,SLOT(changePreviewData()));

    connect(_apply_threshold,SIGNAL(stateChanged(int)),this,SLOT(clipPreview(int)));

    connect(_preview_frame,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));
    connect(_preview_frame_number,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));

    // The action buttons clicking
    connect(_actions,SIGNAL(clicked(QAbstractButton*)),this,SLOT(doActions(QAbstractButton*)));
}

void FramePeakFinder::changeSelectedFrame(int selected_frame)
{
    _preview_frame->setValue(selected_frame);
    _preview_frame_number->setValue(selected_frame);

    preview();
}

void FramePeakFinder::accept()
{
    auto peaks_item = _experiment_item->peaksItem();

    auto peaks_model = dynamic_cast<CollectedPeaksModel*>(_peaks->model());

    auto peaks = peaks_model->peaks();

    if (peaks.empty()) {
        return;
    }

    nsx::PeakList found_peaks;
    found_peaks.reserve(peaks.size());

    for (auto peak : peaks) {
        if (peak->selected()) {
            found_peaks.push_back(peak);
        }
    }

    auto item = new PeakListItem(found_peaks);
    item->setText("Found peaks");
    peaks_item->appendRow(item);

    close();
}

void FramePeakFinder::setColorMap(const std::string &name)
{
    _colormap = std::unique_ptr<ColorMap>(new ColorMap(name));
}

void FramePeakFinder::run()
{
    nsx::info() << "Peak find algorithm: Searching peaks in " << _preview_data->count() << " files";

    // reset progress handler
    auto progressHandler = nsx::sptrProgressHandler(new nsx::ProgressHandler);

    nsx::DataList data;
    for (int i = 0; i < _preview_data->count(); ++i) {
        data.push_back(_preview_data->itemData(i,Qt::UserRole).value<nsx::sptrDataSet>());
    }

    nsx::PeakFinder peak_finder;

    // create a pop-up window that will show the progress
    ProgressView* progressView = new ProgressView(nullptr);
    progressView->watch(progressHandler);

    peak_finder.setHandler(progressHandler);

    peak_finder.setMinSize(_min_blob_size->value());
    peak_finder.setMaxSize(_max_blob_size->value());
    peak_finder.setMaxFrames(_max_blob_width->value());

    // Get the current convolver type
    std::string convolver_type = _convolution_kernels->currentText().toStdString();

    // Get the corresponding parameters
    auto&& parameters = convolutionParameters();

    // Propagate changes to peak finder
    peak_finder.setConvolver(convolver_type,parameters);

    nsx::PeakList peaks;

    // execute in a try-block because the progress handler may throw if it is aborted by GUI
    try {
        peaks = peak_finder.find(data);
    }
    catch(std::exception& e) {
        nsx::debug() << "Caught exception during peak find: " << e.what();
        return;
    }

    // integrate peaks
    for (auto d : data) {
        nsx::PixelSumIntegrator integrator(true, true);
        integrator.integrate(peaks, d, _peak_scale->value(), _background_begin_scale->value(), _background_end_scale->value());
    }

    // delete the progressView
    delete progressView;

    auto peaks_model = dynamic_cast<CollectedPeaksModel*>(_peaks->model());
    peaks_model->setPeaks(peaks);

    nsx::info() << "Peak search complete., found " << peaks.size() << " peaks.";
}

std::map<std::string,double> FramePeakFinder::convolutionParameters() const
{
    std::map<std::string,double> parameters;

    for (int i = 0; i < _convolution_parameters->rowCount(); ++i) {
        std::string pname = _convolution_parameters->item(i,0)->text().toStdString();
        double pvalue = _convolution_parameters->item(i,1)->text().toDouble();
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
    auto convolution_kernel_name = _convolution_kernels->currentText().toStdString();

    nsx::ConvolverFactory convolution_kernel_factory;

    auto convolution_kernel = convolution_kernel_factory.create(convolution_kernel_name,{});

    // Get its corresponding parameters
    const std::map<std::string, double>& parameters = convolution_kernel->parameters();

    disconnect(_convolution_parameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeConvolutionParameters(int,int)));

    _convolution_parameters->setRowCount(0);

    // Iterate through parameters to build the tree
    for (auto p : parameters) {
        int current_row = _convolution_parameters->rowCount();

        _convolution_parameters->insertRow(current_row);

        QTableWidgetItem* pname = new QTableWidgetItem();
        pname->setText(QString::fromStdString(p.first));
        _convolution_parameters->setItem(current_row,0,pname);

        pname->setFlags(pname->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem* pvalue = new QTableWidgetItem();
        pvalue->setText(QString::number(p.second));
        _convolution_parameters->setItem(current_row,1,pvalue);
    }

    _convolution_parameters->horizontalHeader()->setStretchLastSection(true);

    connect(_convolution_parameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeConvolutionParameters(int,int)));
}

void FramePeakFinder::changePreviewData()
{
    auto data = _preview_data->currentData(Qt::UserRole).value<nsx::sptrDataSet>();

    _preview_frame->setMinimum(0);
    _preview_frame->setMaximum(data->nFrames());
    _preview_frame->setValue(0);

    _preview_frame_number->setMinimum(0);
    _preview_frame_number->setMaximum(data->nFrames());
    _preview_frame_number->setValue(0);

    preview();
}

void FramePeakFinder::preview()
{
    auto data = _preview_data->currentData(Qt::UserRole).value<nsx::sptrDataSet>();

    int selected_frame = _preview_frame->value();

    int nrows = data->nRows();
    int ncols = data->nCols();

    std::string convolver_type = _convolution_kernels->currentText().toStdString();
    auto&& convolver_parameters = convolutionParameters();

    Eigen::MatrixXd convolved_frame = data->convolvedFrame(selected_frame,convolver_type, convolver_parameters);

    // apply threshold in preview
    if (_apply_threshold->isChecked()) {
        double threshold_value = _threshold->value();

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
        _pixmap = _preview->scene()->addPixmap(QPixmap::fromImage(image));
    } else {
        _pixmap->setPixmap(QPixmap::fromImage(image));
    }

    _preview->fitInView(_preview->scene()->sceneRect());
}

void FramePeakFinder::clipPreview(int state) {
    Q_UNUSED(state)

    preview();
}

void FramePeakFinder::showEvent(QShowEvent* event)
{
    Q_UNUSED(event)

    _preview->fitInView(_preview->scene()->sceneRect());
}

void FramePeakFinder::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)

    _preview->fitInView(_preview->scene()->sceneRect());

    NSXQFrame::resizeEvent(event);
}

void FramePeakFinder::changeMinBlobSize()
{
    auto&& value = _min_blob_size->value();

    _min_blob_size->setValue(std::min(value,_max_blob_size->value()));
}

void FramePeakFinder::changeMaxBlobSize()
{
    auto&& value = _max_blob_size->value();

    _max_blob_size->setValue(std::max(value,_min_blob_size->value()));
}

void FramePeakFinder::changePeakScale()
{
    auto&& value = _peak_scale->value();

    _peak_scale->setValue(std::min(value,_background_begin_scale->value()));
}

void FramePeakFinder::changeBackgroundBegin()
{
    auto&& value = _background_begin_scale->value();

    value = std::max(value,_peak_scale->value());

    _background_begin_scale->setValue(std::min(value,_background_end_scale->value()));
}

void FramePeakFinder::changeBackgroundEnd()
{
    auto&& value = _background_end_scale->value();

    _background_end_scale->setValue(std::max(value,_background_begin_scale->value()));
}

void FramePeakFinder::doActions(QAbstractButton *button)
{
    auto button_role = _actions->standardButton(button);

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
