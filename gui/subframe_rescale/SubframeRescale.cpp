//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_rescale/SubframeRescale.cpp

//! @brief     Implements class SubframeRescale
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_rescale/SubframeRescale.h"

#include "core/experiment/Experiment.h"
#include "core/rescale/Rescaler.h"
#include "gui/models/Project.h"
#include "gui/utility/ScienceSpinBox.h"
#include "gui/utility/PeakComboBox.h"
#include "gui/utility/ScienceSpinBox.h"
#include "gui/views/PeakTableView.h"
#include "gui/models/Session.h"
#include "gui/graphics/SXPlot.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PeakComboBox.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/Spoiler.h"
#include "tables/crystal/UnitCell.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSplitter>
#include <QVBoxLayout>
#include <qchar.h>
#include <qpushbutton.h>


SubframeRescale::SubframeRescale() : QWidget()
{
    auto* main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout();

    setDataUp();
    setRescalerUp();
    setPeakTableUp();
    setPlotUp();

    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);

    // _right_element->setStretchFactor(0, 2);
    // _right_element->setStretchFactor(1, 1);
}

void SubframeRescale::setDataUp()
{
    Spoiler* _data_box = new Spoiler("Input");
    GridFiller f(_data_box);

    _peak_combo = f.addPeakCombo(ComboType::IntegratedPeaks, "Peak Collection:");

    _left_layout->addWidget(_data_box);
    _data_box->setExpanded(true);
}

void SubframeRescale::setRescalerUp()
{
    Spoiler* rescaler_para = new Spoiler("Rescaler parameters");
    GridFiller f(rescaler_para, true);

    std::tie(_d_min_spin, _d_max_spin) = f.addDoubleSpinBoxPair("Resolution (d) range");
    _space_group_combo = f.addCombo("Space group:");
    _friedel_check = f.addCheckBox(
        "Include Friedel", "Include Freidel relations when merging");
    _profile_intensity_check = f.addCheckBox(
        "Profile intensities", "Use profile intensities instead of sum intensities");
    _ftol_spin = f.addScienceSpinBox(
        "Objective function tolerance",
        "Convergence threshold for objective function evaluations");
    _xtol_spin = f.addScienceSpinBox(
        "Parameter tolerance",
        "Convergence threshold for optimised parameters (scale factors)");
    _ctol_spin = f.addScienceSpinBox(
        "Constraint tolerance",
        "Convergence threshold for optimised parameters (scale factors)");
    _max_iter_spin = f.addSpinBox(
        "Maximum iterations",
        "Maximum number of iterations for optimizer");
    _init_step_spin = f.addDoubleSpinBox(
        "Initial step", "Initial step size for gradient calculation");
    _frame_ratio_spin = f.addDoubleSpinBox(
        "Scale difference", "Maximum difference in scale factor between adjacent images");
    _rescale_button = f.addButton("Rescale");

    _max_iter_spin->setMaximum(10000000000);
    _max_iter_spin->setMinimum(0);

    connect(_rescale_button, &QPushButton::clicked, this, &SubframeRescale::rescale);

    _left_layout->addWidget(rescaler_para);
}

void SubframeRescale::setPeakTableUp()
{
    QGroupBox* peak_group = new QGroupBox("Peaks");
    QGridLayout* peak_grid = new QGridLayout(peak_group);

    peak_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _peak_table = new PeakTableView(this);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->setModel(&_peak_collection_model);
    _peak_table->resizeColumnsToContents();

    _peak_table->setColumnHidden(PeakColumn::BkgGradient, true);
    _peak_table->setColumnHidden(PeakColumn::BkgGradientSigma, true);
    _peak_table->setColumnHidden(PeakColumn::uc, true);
    _peak_table->setColumnHidden(PeakColumn::DataSet, true);
    _peak_table->setColumnHidden(PeakColumn::Rejection, true);
    _peak_table->setColumnHidden(PeakColumn::Filtered, true);
    _peak_table->setColumnHidden(PeakColumn::Enabled, true);

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeRescale::setPlotUp()
{
    _plot = new SXPlot;
    _right_element->addWidget(_plot);
}

void SubframeRescale::refreshAll()
{
    if (!gSession->hasProject())
        return;

    refreshSpaceGroupCombo();
    refreshPeakTable();
    grabRescalerParameters();
    toggleUnsafeWidgets();
}

void SubframeRescale::grabRescalerParameters()
{
    if (!gSession->hasProject())
        return;

    auto* experiment = gSession->currentProject()->experiment();
    auto* params = experiment->rescaler()->parameters();

    _friedel_check->setChecked(params->friedel);
    _profile_intensity_check->setChecked(!params->sum_intensity);
    _ftol_spin->setValue(params->ftol);
    _xtol_spin->setValue(params->xtol);
    _ctol_spin->setValue(params->ctol);
    _max_iter_spin->setValue(params->max_iter);
    _init_step_spin->setValue(params->init_step);
    _frame_ratio_spin->setValue(params->frame_ratio);
}

void SubframeRescale::setRescalerParameters()
{
    if (!gSession->hasProject())
        return;

    auto* experiment = gSession->currentProject()->experiment();
    auto* params = experiment->rescaler()->parameters();

    params->friedel = _friedel_check->isChecked();
    params->sum_intensity = !_profile_intensity_check->isChecked();
    params->ftol = _ftol_spin->value();
    params->xtol = _xtol_spin->value();
    params->ctol = _ctol_spin->value();
    params->max_iter = _max_iter_spin->value();
    params->init_step = _init_step_spin->value();
    params->frame_ratio = _frame_ratio_spin->value();
}


void SubframeRescale::refreshSpaceGroupCombo()
{
    if (!gSession->hasProject())
        return;

    if (!gSession->currentProject()->hasUnitCell())
        return;

    QSignalBlocker blocker(_space_group_combo);
    auto* expt = gSession->currentProject()->experiment();

    if (!gSession->currentProject()->hasUnitCell())
        return;

    std::vector<ohkl::UnitCell*> cells = expt->getUnitCells();
    std::map<std::string, int> space_groups;
    for (auto* cell : cells) {
        if (space_groups.find(cell->spaceGroup().toString()) != space_groups.end())
            ++space_groups[cell->spaceGroup().toString()];
        else
            space_groups[cell->spaceGroup().toString()] = 0;
    }
    std::vector<std::pair<std::string, int>> vec;
    for (const auto& item : space_groups)
        vec.emplace_back(item);
    std::sort(vec.begin(), vec.end(), [](const auto& x, const auto& y) {
        return x.second > y.second;
    });
    _space_group_combo->clear(); // clear first?
    for (const auto& [key, value] : vec)
        _space_group_combo->addItem(QString::fromStdString(key));
    _space_group_combo->setCurrentIndex(0);
}

void SubframeRescale::refreshPeakTable()
{
    if (!gSession->currentProject()->hasPeakCollection())
        return;

    _peak_collection = _peak_combo->currentPeakCollection();
    _peak_collection_item.setPeakCollection(_peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();
}

void SubframeRescale::toggleUnsafeWidgets()
{

    if (!gSession->hasProject())
        return;

    _rescale_button->setEnabled(gSession->currentProject()->hasPeakCollection());
}

void SubframeRescale::rescale()
{
    _scale_factors.clear();
    _image_numbers.clear();

    auto* experiment = gSession->currentProject()->experiment();
    auto* rescaler = experiment->rescaler();

    setRescalerParameters();

    if (_space_group_combo->currentText().toStdString().empty())
        return;
    ohkl::SpaceGroup group = {_space_group_combo->currentText().toStdString()};

    rescaler->setPeakCollection(_peak_combo->currentPeakCollection(), group);
    std::optional<double> minf = rescaler->rescale();
    if (minf) {
        int image = 0;
        for (const double& scale : rescaler->scaleFactors()) {
            _scale_factors.push_back(scale);
            _image_numbers.push_back(++image);
        }
        plotScaleFactors();
    }
    refreshPeakTable();
}

void SubframeRescale::plotScaleFactors()
{
    _plot->clearGraphs();
    QPen pen;
    pen.setColor(QColor("black"));
    pen.setWidth(2.0);

    _plot->addGraph();
    _plot->graph(0)->addData(_image_numbers, _scale_factors);
    _plot->xAxis->setLabel("Image number");
    _plot->yAxis->setLabel("Scale factor");
    _plot->setNotAntialiasedElements(QCP::aeAll);
    _plot->setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend
        | QCP::iSelectPlottables);
    _plot->rescaleAxes();
    _plot->replot();
}
