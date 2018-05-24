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
    std::map<std::string,nsx::sptrDataSet>  datamap=_caller->experiment()->getData();
    nsx::DataList datav;

    auto func = [&](std::pair<std::string,nsx::sptrDataSet> value){datav.push_back(value.second);};

    std::for_each(datamap.begin(), datamap.end(), func);

    CollectedPeaksModel *model = new CollectedPeaksModel(_caller->experiment());

    nsx::PeakList data_peaks;

    for (auto peak: _caller->peaks()) {
        data_peaks.push_back(peak);
    }

    model->setPeaks(data_peaks);
    model->setUnitCells(_caller->experiment()->diffractometer()->getSample()->unitCells());
    ui->tableView->setModel(model);

    //Connect search box
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),ui->tableView,SLOT(showPeaksMatchingText(QString)));


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
