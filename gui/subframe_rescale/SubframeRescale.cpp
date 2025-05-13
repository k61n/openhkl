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
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PeakComboBox.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/Spoiler.h"
#include "tables/crystal/UnitCell.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QSignalBlocker>
#include <QSplitter>
#include <QVBoxLayout>


SubframeRescale::SubframeRescale() : QWidget()
{
    auto* main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout();

    setDataUp();
    setRescalerUp();

    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);

    _right_element->setStretchFactor(0, 2);
    _right_element->setStretchFactor(1, 1);
}

void SubframeRescale::setDataUp()
{
    Spoiler* _data_box = new Spoiler("Input");
    GridFiller f(_data_box);

    _peak_combo_1 = f.addPeakCombo(ComboType::IntegratedPeaks, "Peak Collection 1:");
    _peak_combo_2 = f.addPeakCombo(ComboType::IntegratedPeaks, "Peak Collection 2:");

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
    _ftol_spin = f.addDoubleSpinBox(
        "Objective function tolerance",
        "Convergence threshold for objective function evaluations");
    _ftol_spin = f.addDoubleSpinBox(
        "Parameter tolerance",
        "Convergence threshold for optimised parameters (scale factors)");
    _ctol_spin = f.addDoubleSpinBox(
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

    _left_layout->addWidget(rescaler_para);
}

void SubframeRescale::refreshAll()
{
    if (!gSession->hasProject())
        return;

    toggleUnsafeWidgets();
}

void SubframeRescale::toggleUnsafeWidgets()
{

    if (!gSession->hasProject())
        return;
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
