//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/tree/PeakListPropertyWidget.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <memory>

#include <QSortFilterProxyModel>

#include "core/experiment/DataSet.h"
#include "core/experiment/DataTypes.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"

#include "apps/models/ExperimentItem.h"
#include "apps/models/InstrumentItem.h"
#include "apps/models/PeakListItem.h"
#include "apps/models/SampleItem.h"
#include "apps/models/UnitCellItem.h"
#include "apps/tree/PeakListPropertyWidget.h"
#include "apps/views/PeakTableView.h"

#include "ui_PeakListPropertyWidget.h"

PeakListPropertyWidget::PeakListPropertyWidget(PeakListItem* caller, QWidget* parent)
    : QWidget(parent), _caller(caller), ui(new Ui::PeakListPropertyWidget)
{
    ui->setupUi(this);

    nsx::PeakList peaks;

    for (auto peak : _caller->peaks())
        peaks.push_back(peak);

    _peaks_model = new CollectedPeaksModel(_caller->model(), _caller->experiment(), peaks);

    ui->tableView->setModel(_peaks_model);

    // todo: fix shape library!!
    // connect(ui->tableView, SIGNAL(updateShapeLibrary(nsx::sptrShapeLibrary)),
    // _session.get(), SLOT(updateShapeLibrary(nsx::sptrShapeLibrary)));
}

PeakListPropertyWidget::~PeakListPropertyWidget()
{
    delete ui;
}

CollectedPeaksModel* PeakListPropertyWidget::model()
{
    return _peaks_model;
}
