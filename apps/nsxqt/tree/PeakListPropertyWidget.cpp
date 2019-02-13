#include <memory>

#include <QSortFilterProxyModel>

#include <core/DataSet.h>
#include <core/DataTypes.h>
#include <core/Diffractometer.h>
#include <core/Sample.h>

#include "ExperimentItem.h"
#include "InstrumentItem.h"
#include "PeakListItem.h"
#include "PeakListPropertyWidget.h"
#include "PeakTableView.h"
#include "SampleItem.h"
#include "UnitCellItem.h"

#include "ui_PeakListPropertyWidget.h"

PeakListPropertyWidget::PeakListPropertyWidget(PeakListItem* caller, QWidget *parent)
: QWidget(parent),
  _caller(caller),
  ui(new Ui::PeakListPropertyWidget)
{
    ui->setupUi(this);

    nsx::PeakList peaks;

    for (auto peak: _caller->peaks()) {
        peaks.push_back(peak);
    }

    _peaks_model = new CollectedPeaksModel(_caller->model(),_caller->experiment(), peaks);

    ui->tableView->setModel(_peaks_model);

    // todo: fix shape library!!
    // connect(ui->tableView, SIGNAL(updateShapeLibrary(nsx::sptrShapeLibrary)), _session.get(), SLOT(updateShapeLibrary(nsx::sptrShapeLibrary)));
}

PeakListPropertyWidget::~PeakListPropertyWidget()
{
    delete ui;
}

CollectedPeaksModel* PeakListPropertyWidget::model()
{
    return _peaks_model;
}
