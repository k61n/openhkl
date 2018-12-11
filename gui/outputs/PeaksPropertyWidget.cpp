#include <memory>

#include <QSortFilterProxyModel>

#include <core/DataSet.h>
#include <core/DataTypes.h>
#include <core/Diffractometer.h>
#include <core/Sample.h>

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
