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

#include <nsxlib/ConvolverFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/Logger.h>
#include <nsxlib/MathematicsTypes.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/Threshold.h>
#include <nsxlib/ThresholdFactory.h>

#include "ColorMap.h"
#include "DialogPeakFind.h"

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

DialogPeakFind::DialogPeakFind(const nsx::DataList& data,nsx::sptrPeakFinder peakFinder,QWidget *parent)
: QDialog(parent),
  ui(new Ui::DialogPeakFind),
  _pxmapPreview(nullptr),
  _data(data),
  _colormap(new ColorMap)
{
    ui->setupUi(this);

    // disable resizing
    this->setFixedSize(this->size());

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

    _peakFinder = peakFinder;

    // flip image vertically to conform with DetectorScene
    ui->preview->scale(1, -1);

    // Set the delegate that will force a numeric input for the threshold parameters value column
    ui->thresholdParameters->setColumnCount(2);
    ui->thresholdParameters->horizontalHeader()->hide();
    ui->thresholdParameters->verticalHeader()->hide();
    DoubleDelegate* threshold_table_delegate = new DoubleDelegate();
    ui->thresholdParameters->setItemDelegateForColumn(1,threshold_table_delegate);
    // Set the threshold combo box and table
    ui->threshold->clear();
    nsx::ThresholdFactory threshold_factory;
    for (auto& k : threshold_factory.callbacks()) {
        ui->threshold->addItem(QString::fromStdString(k.first));
    }
    ui->threshold->setCurrentText(_peakFinder->threshold()->name());

    // update dialog with list of parameters
    buildThresholdParametersList();

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

    connect(ui->threshold,SIGNAL(currentIndexChanged(QString)),this,SLOT(changeThreshold(QString)));
    connect(ui->thresholdParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeThresholdParameters(int,int)));
    connect(ui->applyThreshold,SIGNAL(stateChanged(int)),this,SLOT(clipPreview(int)));

    connect(ui->convolver,SIGNAL(currentIndexChanged(QString)),this,SLOT(changeConvolver(QString)));
    connect(ui->convolverParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeConvolverParameters(int,int)));

    connect(ui->minSize,SIGNAL(valueChanged(int)),this,SLOT(changeMinSize(int)));
    connect(ui->maxSize,SIGNAL(valueChanged(int)),this,SLOT(changeMaxSize(int)));
    connect(ui->maxFrames,SIGNAL(valueChanged(int)),this,SLOT(changeMaxFrames(int)));

    connect(ui->dataList,SIGNAL(currentRowChanged(int)),this,SLOT(changeSelectedData(int)));
    connect(ui->frameSlider,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));
    connect(ui->frameIndex,SIGNAL(valueChanged(int)),this,SLOT(changeSelectedFrame(int)));
}

DialogPeakFind::~DialogPeakFind()
{
    delete ui;
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

int DialogPeakFind::exec()
{
    return QDialog::exec();
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

    int selected_frame = ui->frameSlider->value();

    auto frame = data->frame(selected_frame);

    int max_data = frame.maxCoeff();

    int nrows = data->nRows();

    int ncols = data->nCols();

    std::string convolver_type = ui->convolver->currentText().toStdString();
    auto convolver_parameters = convolverParameters();

    Eigen::MatrixXi convolved_frame = data->convolvedFrame(selected_frame,convolver_type, convolver_parameters);

    // apply threshold in preview
    if (ui->applyThreshold->isChecked()) {
        nsx::ThresholdFactory threshold_factory;
        std::string threshold_type = ui->threshold->currentText().toStdString();
        auto threshold_parameters = thresholdParameters();
        auto threshold = threshold_factory.create(threshold_type,threshold_parameters);
        double threshold_value = threshold->value(data,selected_frame);

        for (int i = 0; i < nrows; ++i) {
            for (int j = 0; j < ncols; ++j) {
                convolved_frame(i, j) = convolved_frame(i, j) < threshold_value ? 0 : max_data-1;
            }
        }
    }

    // clamp the result for the preview window
    double minVal = convolved_frame.minCoeff();
    double maxVal = convolved_frame.maxCoeff();
    convolved_frame.array() -= minVal;
    convolved_frame.array() *= static_cast<double>(max_data)/(maxVal-minVal);

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


void DialogPeakFind::clipPreview(int state) {
    Q_UNUSED(state)

    updatePreview();
}

void DialogPeakFind::changeSearchConfidenceValue(double value)
{
    _peakFinder->setSearchConfidence(value);
}

void DialogPeakFind::changeIntegrationConfidenceValue(double value)
{
    _peakFinder->setIntegrationConfidence(value);
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

void DialogPeakFind::changeThreshold(QString threshold_type)
{
    _peakFinder->setThreshold(threshold_type.toStdString(),{});

    // update dialog with list of parameters
    buildThresholdParametersList();

    updatePreview();
}

void DialogPeakFind::changeThresholdParameters(int row, int col)
{
    Q_UNUSED(row)
    Q_UNUSED(col)

    // Get the current threshold type
    std::string threshold_type = ui->threshold->currentText().toStdString();

    // Get the updated parameters
    auto parameters = thresholdParameters();

    // propagate changes to peak finder
    _peakFinder->setThreshold(threshold_type,parameters);

    // update the preview
    updatePreview();
}

std::map<std::string,double> DialogPeakFind::thresholdParameters() const
{
    std::map<std::string,double> parameters;

    for (int i = 0; i < ui->thresholdParameters->rowCount(); ++i) {
        std::string pname = ui->thresholdParameters->item(i,0)->text().toStdString();
        double pvalue = ui->thresholdParameters->item(i,1)->text().toDouble();
        parameters.insert(std::make_pair(pname,pvalue));
    }

    return parameters;
}

void DialogPeakFind::buildThresholdParametersList()
{
    // get the selected threshold
    auto threshold = _peakFinder->threshold();

    // get parameters
    const std::map<std::string, double>& parameters = threshold->parameters();

    disconnect(ui->thresholdParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeThresholdParameters(int,int)));

    ui->thresholdParameters->setRowCount(0);

    // iterate through parameters to build the tree
    for (auto p : parameters) {
        int current_row = ui->thresholdParameters->rowCount();

        ui->thresholdParameters->insertRow(current_row);

        QTableWidgetItem* pname = new QTableWidgetItem();
        pname->setText(QString::fromStdString(p.first));
        ui->thresholdParameters->setItem(current_row,0,pname);

        pname->setFlags(pname->flags() ^ Qt::ItemIsEditable);

        QTableWidgetItem* pvalue = new QTableWidgetItem();
        pvalue->setText(QString::number(p.second));
        ui->thresholdParameters->setItem(current_row,1,pvalue);
    }

    connect(ui->thresholdParameters,SIGNAL(cellChanged(int,int)),this,SLOT(changeThresholdParameters(int,int)));
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
