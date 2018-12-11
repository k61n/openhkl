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

PeakListPropertyWidget::PeakListPropertyWidget(PeakListItem* peak_list_item, QWidget *parent)
: QWidget(parent),
  _peak_list_item(peak_list_item),
  _ui(new Ui::PeakListPropertyWidget)
{
    _ui->setupUi(this);

    nsx::PeakList peaks;

    for (auto peak: peak_list_item->peaks()) {
        peaks.push_back(peak);
    }

    auto* peaks_model = new CollectedPeaksModel(peak_list_item->model(),peak_list_item->experiment(), peaks, this);

    _ui->tableView->setModel(peaks_model);

    // todo: fix shape library!!
    // connect(ui->tableView, SIGNAL(updateShapeLibrary(nsx::sptrShapeLibrary)), _session.get(), SLOT(updateShapeLibrary(nsx::sptrShapeLibrary)));
}

PeakListPropertyWidget::~PeakListPropertyWidget()
{
    delete _ui;
}

CollectedPeaksModel* PeakListPropertyWidget::model()
{
    return dynamic_cast<CollectedPeaksModel*>(_ui->tableView->model());
}
