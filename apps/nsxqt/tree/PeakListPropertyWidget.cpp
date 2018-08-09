#include <memory>

#include <QSortFilterProxyModel>

#include <nsxlib/DataSet.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Sample.h>

#include "CollectedPeaksModel.h"
#include "ExperimentItem.h"
#include "InstrumentItem.h"
#include "PeakListItem.h"
#include "PeakListPropertyWidget.h"
#include "PeakTableView.h"
#include "SampleItem.h"
#include "UnitCellItem.h"

#include "ui_PeakListPropertyWidget.h"

PeakListPropertyWidget::PeakListPropertyWidget(PeakListItem* caller, QWidget *parent) :
     QWidget(parent),
     _caller(caller),
     ui(new Ui::PeakListPropertyWidget)
{
    ui->setupUi(this);
    std::map<std::string,nsx::sptrDataSet>  datamap=_caller->experiment()->data();
    nsx::DataList datav;

    auto func = [&](std::pair<std::string,nsx::sptrDataSet> value){datav.push_back(value.second);};

    std::for_each(datamap.begin(), datamap.end(), func);

    nsx::PeakList peaks;

    for (auto peak: _caller->peaks()) {
        peaks.push_back(peak);
    }

    CollectedPeaksModel *model = new CollectedPeaksModel(_caller->experiment(), peaks);

    ui->tableView->setModel(model);

    // todo: fix shape library!!
    // connect(ui->tableView, SIGNAL(updateShapeLibrary(nsx::sptrShapeLibrary)), _session.get(), SLOT(updateShapeLibrary(nsx::sptrShapeLibrary)));
}

PeakListPropertyWidget::~PeakListPropertyWidget()
{
    delete ui;
}

PeakTableView* PeakListPropertyWidget::getPeakTableView() const
{
    return ui->tableView;
}
