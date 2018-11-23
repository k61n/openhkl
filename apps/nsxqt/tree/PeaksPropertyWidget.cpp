#include <memory>

#include <QSortFilterProxyModel>

#include <nsxlib/DataSet.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Sample.h>

#include "ExperimentItem.h"
#include "InstrumentItem.h"
#include "PeaksItem.h"
#include "PeaksPropertyWidget.h"
#include "PeakTableView.h"
#include "SampleItem.h"
#include "UnitCellItem.h"

#include "ui_PeaksPropertyWidget.h"

PeaksPropertyWidget::PeaksPropertyWidget(PeaksItem* peaks_item, QWidget *parent)
: QWidget(parent),
  _peaks_item(peaks_item),
  ui(new Ui::PeaksPropertyWidget)
{
    ui->setupUi(this);

    auto* peaks_model = new CollectedPeaksModel(_peaks_item->model(),_peaks_item->experiment(), _peaks_item->selectedPeaks(), this);

    ui->tableView->setModel(peaks_model);
}

PeaksPropertyWidget::~PeaksPropertyWidget()
{
    delete ui;
}

CollectedPeaksModel* PeaksPropertyWidget::model()
{
    return dynamic_cast<CollectedPeaksModel*>(ui->tableView->model());
}
