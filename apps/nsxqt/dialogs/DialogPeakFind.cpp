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
#include <nsxlib/PeakFinder.h>
#include <nsxlib/ShapeIntegrator.h>

#include "ColorMap.h"
#include "DialogPeakFind.h"
#include "ProgressView.h"

#include "ui_DialogPeakFind.h"

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

DialogPeakFind::DialogPeakFind(const nsx::DataList& data, QWidget *parent)
: QDialog(parent),
  ui(new Ui::DialogPeakFind),
  _pxmapPreview(nullptr),
  _data(data),
  _peakFinder(new nsx::PeakFinder()),
  _colormap(new ColorMap)
{
    ui->setupUi(this);

    for (auto d : _data) {
        QFileInfo fileinfo(QString::fromStdString(d->filename()));
        ui->dataList->addItem(fileinfo.baseName());
    }

    ui->dataList->setCurrentRow(0);

    ui->frameSlider->setMinimum(0);
    ui->frameSlider->setMaximum(_data[0]->nFrames());

    ui->frameIndex->setMinimum(0);
    ui->frameIndex->setMaximum(_data[0]->nFrames());

    _scene = new QGraphicsScene(this);
    ui->preview->setScene(_scene);

    // flip image vertically to conform with DetectorScene
    ui->preview->scale(1, -1);

    // Set the filter combo box and table
    ui->convolverParameters->setColumnCount(2);
    ui->convolverParameters->horizontalHeader()->hide();
    ui->convolverParameters->verticalHeader()->hide();

    // Set the delegate that will force a numeric input for the convolution kernel parameters value column
    DoubleDelegate* convolver_table_delegate = new DoubleDelegate();
    ui->convolverParameters->setItemDelegateForColumn(1,convolver_table_delegate);

    ui->convolver->clear();
    nsx::ConvolverFactory convolver_factory;
    for (auto& k : convolver_factory.callbacks()) {
        ui->convolver->addItem(QString::fromStdString(k.first));
    }
    ui->convolver->setCurrentText(_peakFinder->convolver()->name());

    // Update dialog with the selected convolver parameters
    buildConvolverParametersList();

    updatePreview();

    // note: need cast due to overloads of this method
    auto valueChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);
    connect(ui->searchScale, valueChanged, this, [&] { _peakFinder->setPeakScale(ui->searchScale->value()); });

    connect(ui->threshold,SIGNAL(valueChanged(double)),this,SLOT(changeThreshold()));
    connect(ui->applyThreshold,SIGNAL(stateChanged(int)),this,SLOT(clipPreview(int)));

    connect(ui->convolver,SIGNAL(currentIndexChanged(QString)),this,SLOT(changeConvolver(QString)));
    connect(ui->convolverParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeConvolverParameters(int,int)));

    connect(ui->minSize,SIGNAL(valueChanged(int)),this,SLOT(changeMinSize(int)));
    connect(ui->maxSize,SIGNAL(valueChanged(int)),this,SLOT(changeMaxSize(int)));
    connect(ui->maxFrames,SIGNAL(valueChanged(int)),this,SLOT(changeMaxFrames(int)));

    connect(ui->dataList,SIGNAL(currentRowChanged(int)),this,SLOT(changeSelectedData(int)));
    connect(ui->frameSlider,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));
    connect(ui->frameIndex,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));

    connect(ui->cancelOK, SIGNAL(clicked(QAbstractButton*)),this,SLOT(actionRequested(QAbstractButton*)));
    connect(ui->cancelOK, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->cancelOK, SIGNAL(rejected()), this, SLOT(reject()));
}

DialogPeakFind::~DialogPeakFind()
{
    delete ui;
}

void DialogPeakFind::actionRequested(QAbstractButton *button)
{
    auto button_role = ui->cancelOK->standardButton(button);

    switch(button_role)
    {
    case QDialogButtonBox::StandardButton::Apply: {
        find();
        break;
    }
    default: {
        return;
    }
    }
}

void DialogPeakFind::changeSelectedData(int selected_data)
{
    auto data = _data[selected_data];

    ui->frameIndex->setMinimum(0);
    ui->frameIndex->setMaximum(data->nFrames());
    ui->frameIndex->setValue(0);

    ui->frameSlider->setMinimum(0);
    ui->frameSlider->setMaximum(data->nFrames());
    ui->frameSlider->setValue(0);

    updatePreview();
}

void DialogPeakFind::setColorMap(const std::string &name)
{
    _colormap = std::unique_ptr<ColorMap>(new ColorMap(name));
}

void DialogPeakFind::changeSelectedFrame(int selected_frame)
{
    ui->frameIndex->setValue(selected_frame);
    ui->frameSlider->setValue(selected_frame);

    updatePreview();
}

void DialogPeakFind::updatePreview()
{
    int selected_data = ui->dataList->currentRow();

    auto data = _data[selected_data];

    ui->frameSlider->setMaximum(data->nFrames()-1);
    int selected_frame = ui->frameSlider->value();

    if (selected_frame >= data->nFrames()) {
        selected_frame = data->nFrames()-1;
    }

    auto frame = data->frame(selected_frame);

    int nrows = data->nRows();
    int ncols = data->nCols();

    std::string convolver_type = ui->convolver->currentText().toStdString();
    auto convolver_parameters = convolverParameters();

    Eigen::MatrixXd convolved_frame = data->convolvedFrame(selected_frame,convolver_type, convolver_parameters);

    // apply threshold in preview
    if (ui->applyThreshold->isChecked()) {
        double threshold_value = ui->threshold->value();

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
}

void DialogPeakFind::showEvent(QShowEvent* event)
{
    Q_UNUSED(event)

    ui->preview->fitInView(_scene->sceneRect());
}

void DialogPeakFind::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)

    ui->preview->fitInView(_scene->sceneRect());

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
    _peakFinder->setThreshold(ui->threshold->value());
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
    std::string convolver_type = ui->convolver->currentText().toStdString();

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

    for (int i = 0; i < ui->convolverParameters->rowCount(); ++i) {
        std::string pname = ui->convolverParameters->item(i,0)->text().toStdString();
        double pvalue = ui->convolverParameters->item(i,1)->text().toDouble();
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

    disconnect(ui->convolverParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeConvolverParameters(int,int)));

    ui->convolverParameters->setRowCount(0);

    // Iterate through parameters to build the tree
    for (auto p : parameters) {
        int current_row = ui->convolverParameters->rowCount();

        ui->convolverParameters->insertRow(current_row);

        QTableWidgetItem* pname = new QTableWidgetItem();
        pname->setText(QString::fromStdString(p.first));
        ui->convolverParameters->setItem(current_row,0,pname);

        pname->setFlags(pname->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem* pvalue = new QTableWidgetItem();
        pvalue->setText(QString::number(p.second));
        ui->convolverParameters->setItem(current_row,1,pvalue);
    }

    connect(ui->convolverParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeConvolverParameters(int,int)));
}

double DialogPeakFind::peakScale() const
{
    return ui->peakScale->value();
}

double DialogPeakFind::bkgBegin() const
{
    return ui->bkgBegin->value();
}

double DialogPeakFind::bkgEnd() const
{
    return ui->bkgEnd->value();
}

const nsx::PeakList& DialogPeakFind::peaks() const
{
    return _peaks;
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
        integrator.integrate(_peaks, numor, ui->peakScale->value(), ui->bkgBegin->value(), ui->bkgEnd->value());
    }

    // delete the progressView
    delete progressView;

    nsx::debug() << "Peak search complete., found " << _peaks.size() << " peaks.";
}
