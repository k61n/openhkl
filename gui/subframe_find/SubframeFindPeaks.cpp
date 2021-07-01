//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_find/SubframeFindPeaks.cpp
//! @brief     Implements classes FoundPeaks, SubframeFindPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_find/SubframeFindPeaks.h"

#include "core/convolve/ConvolverFactory.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/raw/IDataReader.h"
#include "gui/dialogs/ListNameDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics/DetectorView.h"
#include "gui/items/PeakItem.h"
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/Spoiler.h"
#include "gui/views/PeakTableView.h"
#include "gui/widgets/PeakViewWidget.h"

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QItemDelegate>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpacerItem>
#include <QTableWidgetItem>

SubframeFindPeaks::SubframeFindPeaks()
    : QWidget()
    , _peak_collection("temp", nsx::listtype::FOUND)
    , _peak_collection_item()
    , _peak_collection_model()
    , _pixmap(nullptr)
    , _size_policy_right(QSizePolicy::Expanding, QSizePolicy::Expanding)
{
    auto main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout(this);

    setDataUp();
    setBlobUp();
    setIntegrateUp();
    setPreviewUp();
    setSaveUp();
    setFigureUp();
    setPeakTableUp();

    _right_element->setSizePolicy(_size_policy_right);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);

    connect(_kernel_combo, &QComboBox::currentTextChanged, [=](QString) {
        updateConvolutionParameters();
        refreshPreview();
    });
}

void SubframeFindPeaks::setDataUp()
{
    Spoiler* _data_box = new Spoiler("Input");
    GridFiller f(_data_box);

    _exp_combo = f.addCombo("Experiment");
    _data_combo = f.addCombo("Data set");
    _all_data = f.addCheckBox("Search all", 1);

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        [=]() {
            grabFinderParameters();
            grabIntegrationParameters();
            updateDatasetList();
        });

    connect(
        _data_combo, &QComboBox::currentTextChanged, this,
        &SubframeFindPeaks::updateDatasetParameters);

    _left_layout->addWidget(_data_box);
}

void SubframeFindPeaks::setBlobUp()
{
    Spoiler* blob_para = new Spoiler("Peak search parameters");
    GridFiller f(blob_para, true);

    _threshold_spin = f.addSpinBox(
        "Threshold", "(counts) - pixels with fewer counts than the threshold are discarded");

    _scale_spin =
        f.addDoubleSpinBox("Merging scale", "(sigmas) - blob scaling factor to detect collisions");

    _min_size_spin = f.addSpinBox(
        "Minimum size", "(integer) - blobs containing fewer points than this count are discarded");

    _max_size_spin = f.addSpinBox(
        "Maximum size", "(integer) - blobs containing more points than this count are discarded");

    _max_width_spin = f.addSpinBox(
        "Maximum width", "(frames) - blob is discarded if it spans more frames than this value");

    _kernel_combo = f.addCombo("Kernel", "Convolution kernel for peak search");

    QLabel* kernel_para_label = new QLabel("Parameters:");
    kernel_para_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    kernel_para_label->setToolTip("r1, r2, r3 parameters for pixel sum integration");
    f.addWidget(kernel_para_label, 0);

    _kernel_para_table = new QTableWidget(this);
    f.addWidget(_kernel_para_table, 0);

    _start_frame_spin = f.addSpinBox("Start frame", "(frame) - start frame for peak finding");

    _end_frame_spin = f.addSpinBox("End frame", "(frame) - end frame for peak finding");

    auto find_button = f.addButton("Find peaks");

    _threshold_spin->setMaximum(10000000);
    _scale_spin->setMaximum(10000000);
    _min_size_spin->setMaximum(10000000);
    _max_size_spin->setMaximum(10000000);
    _max_width_spin->setMaximum(10000000);

    connect(find_button, &QPushButton::clicked, this, &SubframeFindPeaks::find);

    _left_layout->addWidget(blob_para);
}

void SubframeFindPeaks::setIntegrateUp()
{
    Spoiler* integration_para = new Spoiler("Integration parameters");
    GridFiller f(integration_para);

    _peak_area = f.addDoubleSpinBox("Peak end", "(sigmas) - scaling factor for peak region");

    _bkg_lower =
        f.addDoubleSpinBox("Bkg. begin", "(sigmas) - scaling factor for lower limit of background");

    _bkg_upper =
        f.addDoubleSpinBox("Bkg. end", "(sigmas) - scaling factor for upper limit of background");

    auto integrate_button = f.addButton("Integrate");

    _peak_area->setMaximum(10000000);
    _bkg_lower->setMaximum(10000000);
    _bkg_upper->setMaximum(10000000);

    connect(integrate_button, &QPushButton::clicked, this, &SubframeFindPeaks::integrate);

    _left_layout->addWidget(integration_para);
}

void SubframeFindPeaks::setPreviewUp()
{
    Spoiler* preview_spoiler = new Spoiler("Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid Peaks");

    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, this,
        &SubframeFindPeaks::refreshPeakVisual);

    connect(
        _peak_view_widget->set1.peakEnd, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_area, &QDoubleSpinBox::setValue);

    connect(
        _peak_view_widget->set1.bkgBegin, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _bkg_lower, &QDoubleSpinBox::setValue);

    connect(
        _peak_view_widget->set1.bkgEnd, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _bkg_upper, &QDoubleSpinBox::setValue);

    _live_check = new QCheckBox("Apply threshold to preview");
    // _peak_view_widget->addWidget(_live_check, 8, 0, 1, 3);
    // Not sure what the _live_check widget does - zamaan

    preview_spoiler->setContentLayout(*_peak_view_widget);

    _left_layout->addWidget(preview_spoiler);
}

void SubframeFindPeaks::setSaveUp()
{
    auto save_button = new QPushButton("Create peak collection");
    _left_layout->addWidget(save_button);
    connect(save_button, &QPushButton::clicked, this, &SubframeFindPeaks::accept);
}

void SubframeFindPeaks::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Preview");
    QGridLayout* figure_grid = new QGridLayout(figure_group);

    figure_group->setSizePolicy(_size_policy_right);

    _figure_view = new DetectorView(this);
    _figure_view->getScene()->linkPeakModel1(&_peak_collection_model);
    _figure_view->scale(1, -1);
    figure_grid->addWidget(_figure_view, 0, 0, 1, 2);

    _figure_scroll = new QScrollBar(this);
    _figure_scroll->setOrientation(Qt::Horizontal);
    _figure_scroll->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    figure_grid->addWidget(_figure_scroll, 1, 0, 1, 1);

    _figure_spin = new QSpinBox(this);
    _figure_spin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    figure_grid->addWidget(_figure_spin, 1, 1, 1, 1);

    connect(
        _figure_scroll, SIGNAL(valueChanged(int)), _figure_view->getScene(),
        SLOT(slotChangeSelectedFrame(int)));

    connect(_figure_scroll, SIGNAL(valueChanged(int)), _figure_spin, SLOT(setValue(int)));

    connect(_figure_spin, SIGNAL(valueChanged(int)), _figure_scroll, SLOT(setValue(int)));

    connect(
        _figure_view->getScene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframeFindPeaks::changeSelected);

    _right_element->addWidget(figure_group);
}

void SubframeFindPeaks::setPeakTableUp()
{
    QGroupBox* peak_group = new QGroupBox("Peaks");
    QGridLayout* peak_grid = new QGridLayout(peak_group);

    peak_group->setSizePolicy(_size_policy_right);

    _peak_table = new PeakTableView(this);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->setModel(&_peak_collection_model);
    _peak_table->resizeColumnsToContents();

    _peak_table->setColumnHidden(0, true);
    _peak_table->setColumnHidden(1, true);
    _peak_table->setColumnHidden(2, true);

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeFindPeaks::refreshAll()
{
    setParametersUp();
}

void SubframeFindPeaks::setParametersUp()
{
    if (gSession->experimentNames().empty())
        return;

    setExperimentsUp();
    refreshPeakTable();

    _kernel_combo->blockSignals(true);

    _kernel_combo->clear();
    nsx::ConvolverFactory convolver_factory;
    for (const auto& convolution_kernel_combo : convolver_factory.callbacks())
        _kernel_combo->addItem(QString::fromStdString(convolution_kernel_combo.first));

    _kernel_combo->blockSignals(false);

    _kernel_combo->setCurrentText("annular");
}

void SubframeFindPeaks::setExperimentsUp()
{
    _exp_combo->blockSignals(true);
    QString current_exp = _exp_combo->currentText();
    _exp_combo->clear();

    if (!gSession->experimentNames().empty()) {
        for (const QString& exp : gSession->experimentNames())
            _exp_combo->addItem(exp);
        _exp_combo->setCurrentText(current_exp);
        grabFinderParameters();
        grabIntegrationParameters();
        updateDatasetList();
    }
    _exp_combo->blockSignals(false);
}

void SubframeFindPeaks::updateDatasetList()
{
    _data_combo->blockSignals(true);
    QString current_data = _data_combo->currentText();
    _data_combo->clear();

    const QStringList& datanames{gSession->currentProject()->getDataNames()};
    if (!datanames.empty()) {
        _data_combo->addItems(datanames);
        _data_combo->setCurrentText(current_data);
        updateDatasetParameters(_data_combo->currentText());
    }

    _data_combo->blockSignals(false);
}

void SubframeFindPeaks::updateDatasetParameters(const QString& dataname)
{
    nsx::sptrDataSet data =
        gSession->experimentAt(
            _exp_combo->currentIndex())->experiment()->getData(dataname.toStdString());

    _end_frame_spin->setMaximum(data->nFrames());
    _end_frame_spin->setValue(data->nFrames());
    _start_frame_spin->setMaximum(data->nFrames());
    _start_frame_spin->setValue(1);

    _figure_view->getScene()->slotChangeSelectedData(data, _figure_spin->value());
    //_figure_view->getScene()->setMaxIntensity(3000);
    emit _figure_view->getScene()->dataChanged();
    _figure_view->getScene()->update();

    _figure_scroll->setMaximum(data->nFrames() - 1);
    _figure_scroll->setMinimum(0);

    _figure_spin->setMaximum(data->nFrames() - 1);
    _figure_spin->setMinimum(0);
}

void SubframeFindPeaks::grabFinderParameters()
{
    nsx::PeakFinder* finder =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFinder();

    _min_size_spin->setValue(finder->minSize());
    _max_size_spin->setValue(finder->maxSize());
    _scale_spin->setValue(finder->peakEnd());
    _max_width_spin->setValue(finder->maxFrames());
    _start_frame_spin->setValue(finder->framesBegin()+1);
    _end_frame_spin->setValue(finder->framesEnd());
    _threshold_spin->setValue(finder->threshold());

    nsx::Convolver* convolver = finder->convolver();
    std::string convolverType = convolver->type();
    _kernel_combo->setCurrentText(QString::fromStdString(convolverType));

    const std::map<std::string, double>& params = convolver->parameters();
    using mapIterator = std::map<std::string, double>::const_iterator;

    _kernel_para_table->clear();
    _kernel_para_table->setRowCount(0);
    _kernel_para_table->setColumnCount(2);
    int currentRow = 0;
    for (mapIterator it = params.begin(); it != params.end(); ++it) {
        _kernel_para_table->insertRow(currentRow);

        QString name = QString::fromStdString(it->first);
        QTableWidgetItem* pname = new QTableWidgetItem();
        pname->setData(Qt::DisplayRole, name);
        pname->setFlags(pname->flags() ^ Qt::ItemIsEditable);


        double val = it->second;
        QTableWidgetItem* pvalue = new QTableWidgetItem();
        pvalue->setData(Qt::DisplayRole, val);

        _kernel_para_table->setItem(currentRow, 0, pname);
        _kernel_para_table->setItem(currentRow, 1, pvalue);

        currentRow++;
    }
    _kernel_para_table->resizeColumnsToContents();
}

void SubframeFindPeaks::setFinderParameters()
{
    nsx::PeakFinder* finder =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFinder();

    finder->setMinSize(_min_size_spin->value());
    finder->setMaxSize(_max_size_spin->value());
    finder->setPeakEnd(_scale_spin->value());
    finder->setMaxFrames(_max_width_spin->value());
    finder->setFramesBegin(_start_frame_spin->value()-1);
    finder->setFramesEnd(_end_frame_spin->value());
    finder->setThreshold(_threshold_spin->value());

    std::string convolverType = _kernel_combo->currentText().toStdString();
    nsx::ConvolverFactory factory;
    nsx::Convolver* convolver = factory.create(convolverType, {});
    convolver->setParameters(convolutionParameters());
    finder->setConvolver(std::unique_ptr<nsx::Convolver>(convolver));
}

void SubframeFindPeaks::grabIntegrationParameters()
{
    nsx::IPeakIntegrator* integrator = gSession->experimentAt(_exp_combo->currentIndex())
                                           ->experiment()
                                           ->getIntegrator(std::string("Pixel sum integrator"));

    _peak_area->setValue(integrator->peakEnd());
    _bkg_lower->setValue(integrator->backBegin());
    _bkg_upper->setValue(integrator->backEnd());
}

void SubframeFindPeaks::updateConvolutionParameters()
{
    std::string kernelName = _kernel_combo->currentText().toStdString();
    nsx::ConvolverFactory _kernel_comboFactory;
    nsx::Convolver* kernel = _kernel_comboFactory.create(kernelName, {});

    const std::map<std::string, double>& params = kernel->parameters();
    using mapIterator = std::map<std::string, double>::const_iterator;

    _kernel_para_table->setRowCount(0);
    _kernel_para_table->setColumnCount(2);
    int currentRow = 0;
    for (mapIterator it = params.begin(); it != params.end(); ++it) {
        _kernel_para_table->insertRow(currentRow);

        QString name = QString::fromStdString(it->first);
        QTableWidgetItem* pname = new QTableWidgetItem();
        pname->setData(Qt::DisplayRole, name);
        pname->setFlags(pname->flags() ^ Qt::ItemIsEditable);


        QString val = QString::number(it->second);
        QTableWidgetItem* pvalue = new QTableWidgetItem();
        pvalue->setData(Qt::DisplayRole, val);

        _kernel_para_table->setItem(currentRow, 0, pname);
        _kernel_para_table->setItem(currentRow, 1, pvalue);

        currentRow++;
    }
    _kernel_para_table->resizeColumnsToContents();
}

void SubframeFindPeaks::find()
{
    nsx::DataList data_list;
    const nsx::DataList all_data = gSession->experimentAt(_exp_combo->currentIndex())->allData();

    if (_all_data->isChecked()) {
        for (int i = 0; i < all_data.size(); ++i)
            data_list.push_back(all_data.at(i));
    } else {
        data_list.push_back(all_data.at(_data_combo->currentIndex()));
    }

    nsx::PeakFinder* finder =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFinder();
    nsx::sptrProgressHandler progHandler = nsx::sptrProgressHandler(new nsx::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(progHandler);
    finder->setHandler(progHandler);

    setFinderParameters();

    try {
        finder->find(data_list);
        refreshPeakTable();
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
}

void SubframeFindPeaks::integrate()
{
    nsx::Experiment* experiment = gSession->experimentAt(_exp_combo->currentIndex())->experiment();

    nsx::IPeakIntegrator* integrator = experiment->getIntegrator("Pixel sum integrator");

    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(handler);

    nsx::IntegrationParameters params{};
    params.peak_end = _peak_area->value();
    params.bkg_begin = _bkg_lower->value();
    params.bkg_end = _bkg_upper->value();
    integrator->setParameters(params);
    integrator->setHandler(handler);

    experiment->integrateFoundPeaks("Pixel sum integrator");

    refreshPeakTable();
}

std::map<std::string, double> SubframeFindPeaks::convolutionParameters()
{
    std::map<std::string, double> parameters;
    for (int i = 0; i < _kernel_para_table->rowCount(); ++i) {
        std::string pname = _kernel_para_table->item(i, 0)->text().toStdString();
        double pvalue = _kernel_para_table->item(i, 1)->text().toDouble();
        parameters.insert(std::make_pair(pname, pvalue));
    }
    return parameters;
}

void SubframeFindPeaks::accept()
{
    nsx::PeakFinder* finder =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->peakFinder();

    if (!finder->currentPeaks().empty()) {
        std::unique_ptr<ListNameDialog> dlg(new ListNameDialog());
        dlg->exec();
        if (!dlg->listName().isEmpty()) {
            gSession->experimentAt(_exp_combo->currentIndex())
                ->experiment()
                ->acceptFoundPeaks(dlg->listName().toStdString());
            gSession->experimentAt(_exp_combo->currentIndex())->generatePeakModel(dlg->listName());
        }
    }
}

void SubframeFindPeaks::refreshPreview()
{
    nsx::sptrDataSet dataset = _data_combo->currentData().value<nsx::sptrDataSet>();
    int selected = 0;
    int nrows = dataset->nRows();
    int ncols = dataset->nCols();

    std::string convolvertype = _kernel_combo->currentText().toStdString();
    std::map<std::string, double> convolverParams = convolutionParameters();
    Eigen::MatrixXd convolvedFrame =
        nsx::convolvedFrame(dataset->reader()->data(selected), convolvertype, convolverParams);
    if (_live_check->isChecked()) {
        double thresholdVal = _threshold_spin->value();
        for (int i = 0; i < nrows; ++i) {
            for (int j = 0; j < ncols; ++j)
                convolvedFrame(i, j) = convolvedFrame(i, j) < thresholdVal ? 0 : 1;
        }
    }
    double minVal = convolvedFrame.minCoeff();
    double maxVal = convolvedFrame.maxCoeff();
    if (maxVal - minVal <= 0.0)
        maxVal = minVal + 1.0;
    convolvedFrame.array() -= minVal;
    convolvedFrame.array() /= maxVal - minVal;
    QRect rect(0, 0, ncols, nrows);
    ColorMap* m = new ColorMap;
    QImage image = m->matToImage(convolvedFrame.cast<double>(), rect, maxVal);
    if (!_pixmap)
        _pixmap = _figure_view->scene()->addPixmap(QPixmap::fromImage(image));
    else
        _pixmap->setPixmap(QPixmap::fromImage(image));
    _figure_view->fitInView(_figure_view->scene()->sceneRect());
}

void SubframeFindPeaks::refreshPeakTable()
{
    std::vector<nsx::Peak3D*> peaks = gSession->experimentAt(_exp_combo->currentIndex())
                                          ->experiment()
                                          ->peakFinder()
                                          ->currentPeaks();

    _figure_view->getScene()->clearPeakItems();
    _peak_collection.populate(peaks);
    _peak_collection_item.setPeakCollection(&_peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();

    _peak_table->setColumnHidden(0, true);
    _peak_table->setColumnHidden(1, true);
    _peak_table->setColumnHidden(2, true);

    refreshPeakVisual();
}

void SubframeFindPeaks::refreshPeakVisual()
{
    if (_peak_collection_item.childCount() == 0)
        return;

    for (int i = 0; i < _peak_collection_item.childCount(); i++) {
        PeakItem* peak = _peak_collection_item.peakItemAt(i);
        auto graphic = peak->peakGraphic();

        graphic->showLabel(false);
        graphic->setColor(Qt::transparent);
        graphic->initFromPeakViewWidget(
            peak->peak()->enabled() ? _peak_view_widget->set1 : _peak_view_widget->set2);
    }
    _figure_view->getScene()->update();
    _figure_view->getScene()->initIntRegionFromPeakWidget(_peak_view_widget->set1);
    _figure_view->getScene()->drawPeakitems();
}

void SubframeFindPeaks::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}
