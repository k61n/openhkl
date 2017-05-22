#include "ui_PeakListPropertyWidget.h"

#include <memory>

#include <QSortFilterProxyModel>

#include <nsxlib/data/DataSet.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/instrument/Sample.h>

#include "models/CollectedPeaksModel.h"
#include "models/ExperimentItem.h"
#include "models/InstrumentItem.h"
#include "models/PeakListItem.h"
#include "models/SampleItem.h"
#include "models/UnitCellItem.h"
#include "PeakListPropertyWidget.h"
#include "views/PeakTableView.h"

using sptrDataSet = std::shared_ptr<nsx::DataSet>;

PeakListPropertyWidget::PeakListPropertyWidget(PeakListItem* caller, QWidget *parent) :
     QWidget(parent),
     _caller(caller),
     ui(new Ui::PeakListPropertyWidget)
{
    ui->setupUi(this);
    std::map<std::string,sptrDataSet>  datamap=_caller->getExperiment()->getData();
    std::vector<sptrDataSet> datav;

    auto func = [&](std::pair<std::string,sptrDataSet> value){datav.push_back(value.second);};

    std::for_each(datamap.begin(), datamap.end(), func);

    CollectedPeaksModel *model = new CollectedPeaksModel(_caller->getExperiment());
    model->setPeaks(datav);
    model->setUnitCells(_caller->getExperiment()->getDiffractometer()->getSample()->getUnitCells());
    ui->tableView->setModel(model);

    //Connect search box
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),ui->tableView,SLOT(showPeaksMatchingText(QString)));
}

PeakListPropertyWidget::~PeakListPropertyWidget()
{
    delete ui;
}

PeakTableView* PeakListPropertyWidget::getPeakTableView() const
{
    return ui->tableView;
}
