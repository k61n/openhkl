#include <QFileInfo>
#include <QImage>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QItemDelegate>
#include <QSortFilterProxyModel>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTreeView>
#include <QtGlobal>

#include <nsxlib/ConvolverFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/Logger.h>
#include <nsxlib/MathematicsTypes.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/ShapeIntegrator.h>

#include "CollectedPeaksModel.h"
#include "ColorMap.h"
#include "DialogPeakFind.h"
#include "ExperimentItem.h"
#include "PeaksItem.h"
#include "PeakListItem.h"
#include "ProgressView.h"

#include "ui_DialogPeakFind.h"

#include <QDebug>

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

DialogPeakFind* DialogPeakFind::_instance = nullptr;

DialogPeakFind* DialogPeakFind::create(ExperimentItem* experiment_item, const nsx::DataList& data, QWidget* parent)
{
    if (!_instance) {
        _instance = new DialogPeakFind(experiment_item, data, parent);
    }

    return _instance;
}

DialogPeakFind* DialogPeakFind::Instance()
{
    return _instance;
}

DialogPeakFind::DialogPeakFind(ExperimentItem* experiment_item, const nsx::DataList& data, QWidget *parent)
: QDialog(parent),
  _ui(new Ui::DialogPeakFind),
  _experiment_item(experiment_item),
  _pxmapPreview(nullptr),
  _data(data),
  _peakFinder(new nsx::PeakFinder()),
  _colormap(new ColorMap)
{
    _ui->setupUi(this);

    setModal(false);

    setWindowModality(Qt::NonModal);

    setAttribute(Qt::WA_DeleteOnClose);

    setFocusPolicy(Qt::ClickFocus);

    _ui->tabs->setCurrentIndex(0);

    for (auto d : _data) {
        QFileInfo fileinfo(QString::fromStdString(d->filename()));
        _ui->dataList->addItem(fileinfo.baseName());
    }

    _ui->dataList->setCurrentRow(0);

    _ui->frameSlider->setMinimum(0);
    _ui->frameSlider->setMaximum(_data[0]->nFrames());

    _ui->frameIndex->setMinimum(0);
    _ui->frameIndex->setMaximum(_data[0]->nFrames());

    _scene = new QGraphicsScene(_ui->settings_tab);
    _ui->preview->setScene(_scene);

    // flip image vertically to conform with DetectorScene
    _ui->preview->scale(1, -1);

    // Set the filter combo box and table
    _ui->convolverParameters->setColumnCount(2);
    _ui->convolverParameters->horizontalHeader()->hide();
    _ui->convolverParameters->verticalHeader()->hide();

    // Set the delegate that will force a numeric input for the convolution kernel parameters value column
    DoubleDelegate* convolver_table_delegate = new DoubleDelegate();
    _ui->convolverParameters->setItemDelegateForColumn(1,convolver_table_delegate);

    _ui->convolver->clear();
    nsx::ConvolverFactory convolver_factory;
    for (auto& k : convolver_factory.callbacks()) {
        _ui->convolver->addItem(QString::fromStdString(k.first));
    }
    _ui->convolver->setCurrentText(_peakFinder->convolver()->name());

    // Update dialog with the selected convolver parameters
    buildConvolverParametersList();

    updatePreview();

    _peaks_model = new CollectedPeaksModel(_experiment_item->model(),_experiment_item->experiment(),{});
    _ui->peaks->setModel(_peaks_model);

    // note: need cast due to overloads of this method
    auto valueChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);
    connect(_ui->searchScale, valueChanged, this, [&] { _peakFinder->setPeakScale(_ui->searchScale->value()); });

    connect(_ui->threshold,SIGNAL(valueChanged(double)),this,SLOT(changeThreshold()));
    connect(_ui->applyThreshold,SIGNAL(stateChanged(int)),this,SLOT(clipPreview(int)));

    connect(_ui->convolver,SIGNAL(currentIndexChanged(QString)),this,SLOT(changeConvolver(QString)));
    connect(_ui->convolverParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeConvolverParameters(int,int)));

    connect(_ui->minSize,SIGNAL(valueChanged(int)),this,SLOT(changeMinSize(int)));
    connect(_ui->maxSize,SIGNAL(valueChanged(int)),this,SLOT(changeMaxSize(int)));
    connect(_ui->maxFrames,SIGNAL(valueChanged(int)),this,SLOT(changeMaxFrames(int)));

    connect(_ui->dataList,SIGNAL(currentRowChanged(int)),this,SLOT(changeSelectedData(int)));
    connect(_ui->frameSlider,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));
    connect(_ui->frameIndex,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));

    connect(_ui->cancelOK, SIGNAL(clicked(QAbstractButton*)),this,SLOT(actionRequested(QAbstractButton*)));
}

DialogPeakFind::~DialogPeakFind()
{
    delete _ui;

    if (_instance) {
        _instance = nullptr;
    }
}

void DialogPeakFind::actionRequested(QAbstractButton *button)
{
    auto button_role = _ui->cancelOK->standardButton(button);

    switch(button_role)
    {
    case QDialogButtonBox::StandardButton::Apply: {
        find();
        break;
    }
    case QDialogButtonBox::StandardButton::Cancel: {
        reject();
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

void DialogPeakFind::accept()
{
    auto peaks_item = _experiment_item->peaksItem();

    auto peaks = _peaks_model->peaks();

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

    QDialog::accept();
}

void DialogPeakFind::changeSelectedData(int selected_data)
{
    auto data = _data[selected_data];

    _ui->frameIndex->setMinimum(0);
    _ui->frameIndex->setMaximum(data->nFrames());
    _ui->frameIndex->setValue(0);

    _ui->frameSlider->setMinimum(0);
    _ui->frameSlider->setMaximum(data->nFrames());
    _ui->frameSlider->setValue(0);

    updatePreview();
}

void DialogPeakFind::setColorMap(const std::string &name)
{
    _colormap = std::unique_ptr<ColorMap>(new ColorMap(name));
}

void DialogPeakFind::changeSelectedFrame(int selected_frame)
{
    _ui->frameIndex->setValue(selected_frame);
    _ui->frameSlider->setValue(selected_frame);

    updatePreview();
}

void DialogPeakFind::updatePreview()
{
    int selected_data = _ui->dataList->currentRow();

    auto data = _data[selected_data];

    _ui->frameSlider->setMaximum(data->nFrames()-1);
    size_t selected_frame = _ui->frameSlider->value();

    if (selected_frame >= data->nFrames()) {
        selected_frame = data->nFrames()-1;
    }

    int nrows = data->nRows();
    int ncols = data->nCols();

    std::string convolver_type = _ui->convolver->currentText().toStdString();
    auto convolver_parameters = convolverParameters();

    Eigen::MatrixXd convolved_frame = data->convolvedFrame(selected_frame,convolver_type, convolver_parameters);

    // apply threshold in preview
    if (_ui->applyThreshold->isChecked()) {
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

    if (!_pxmapPreview) {
        _pxmapPreview = _scene->addPixmap(QPixmap::fromImage(image));
    } else {
        _pxmapPreview->setPixmap(QPixmap::fromImage(image));
    }

    _ui->preview->fitInView(_scene->sceneRect());
}

void DialogPeakFind::showEvent(QShowEvent* event)
{
    Q_UNUSED(event)

    _ui->preview->fitInView(_scene->sceneRect());
}

void DialogPeakFind::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)

    _ui->preview->fitInView(_scene->sceneRect());

    QDialog::resizeEvent(event);
}

void DialogPeakFind::clipPreview(int state) {
    Q_UNUSED(state)

    updatePreview();
}

void DialogPeakFind::changeMinSize(int size)
{
    _peakFinder->setMinSize(size);
}

void DialogPeakFind::changeMaxSize(int size)
{
    _peakFinder->setMaxSize(size);
}

void DialogPeakFind::changeMaxFrames(int size)
{
    _peakFinder->setMaxFrames(size);
}

void DialogPeakFind::changeThreshold()
{
    _peakFinder->setThreshold(_ui->threshold->value());
    updatePreview();
}

void DialogPeakFind::changeConvolver(QString convolver_type)
{
    // Set the new convolver with default parameters to the peak finder
    _peakFinder->setConvolver(convolver_type.toStdString(),{});

    // Update dialog with the selected convolver parameters
    buildConvolverParametersList();

    // Update the preview
    updatePreview();
}

void DialogPeakFind::changeConvolverParameters(int row, int col)
{
    Q_UNUSED(row)
    Q_UNUSED(col)

    // Get the current convolver type
    std::string convolver_type = _ui->convolver->currentText().toStdString();

    // Get the corresponding parameters
    auto parameters = convolverParameters();

    // Propagate changes to peak finder
    _peakFinder->setConvolver(convolver_type,parameters);

    // Update the preview
    updatePreview();
}

std::map<std::string,double> DialogPeakFind::convolverParameters() const
{
    std::map<std::string,double> parameters;

    for (int i = 0; i < _ui->convolverParameters->rowCount(); ++i) {
        std::string pname = _ui->convolverParameters->item(i,0)->text().toStdString();
        double pvalue = _ui->convolverParameters->item(i,1)->text().toDouble();
        parameters.insert(std::make_pair(pname,pvalue));
    }

    return parameters;
}

void DialogPeakFind::buildConvolverParametersList()
{
    // Get the selected convolver
    auto convolver = _peakFinder->convolver();

    // Get its corresponding parameters
    const std::map<std::string, double>& parameters = convolver->parameters();

    disconnect(_ui->convolverParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeConvolverParameters(int,int)));

    _ui->convolverParameters->setRowCount(0);

    // Iterate through parameters to build the tree
    for (auto p : parameters) {
        int current_row = _ui->convolverParameters->rowCount();

        _ui->convolverParameters->insertRow(current_row);

        QTableWidgetItem* pname = new QTableWidgetItem();
        pname->setText(QString::fromStdString(p.first));
        _ui->convolverParameters->setItem(current_row,0,pname);

        pname->setFlags(pname->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem* pvalue = new QTableWidgetItem();
        pvalue->setText(QString::number(p.second));
        _ui->convolverParameters->setItem(current_row,1,pvalue);
    }

    connect(_ui->convolverParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeConvolverParameters(int,int)));
}

void DialogPeakFind::find()
{
    size_t max = _data.size();

    nsx::info() << "Peak find algorithm: Searching peaks in " << max << " files";

    // reset progress handler
    auto progressHandler = nsx::sptrProgressHandler(new nsx::ProgressHandler);

    // create a pop-up window that will show the progress
    ProgressView* progressView = new ProgressView(nullptr);
    progressView->watch(progressHandler);

    _peakFinder->setHandler(progressHandler);

    _peaks.clear();

    // execute in a try-block because the progress handler may throw if it is aborted by GUI
    try {
        _peaks = _peakFinder->find(_data);
    }
    catch(std::exception& e) {
        nsx::debug() << "Caught exception during peak find: " << e.what();
        return;
    }

    // integrate peaks
    for (auto numor : _data) {
        nsx::PixelSumIntegrator integrator(true, true);
        integrator.integrate(_peaks, numor, _ui->peakScale->value(), _ui->bkgBegin->value(), _ui->bkgEnd->value());
    }

    // delete the progressView
    delete progressView;

    _peaks_model->setPeaks(_peaks);

    nsx::info() << "Peak search complete., found " << _peaks.size() << " peaks.";
}
