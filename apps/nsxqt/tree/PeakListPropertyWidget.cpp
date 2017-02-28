#include <QSortFilterProxyModel>

#include "PeakListPropertyWidget.h"
#include "ui_PeakListPropertyWidget.h"
#include "views/PeakTableView.h"
#include "models/PeakListItem.h"
#include "models/ExperimentItem.h"
#include "models/SampleItem.h"
#include "models/InstrumentItem.h"
#include <nsxlib/data/IData.h>
#include "models/UnitCellItem.h"
#include "models/CollectedPeaksModel.h"

#include <QtDebug>
#include <memory>

using SX::Data::DataSet;

PeakListPropertyWidget::PeakListPropertyWidget(PeakListItem* caller, QWidget *parent) :
     QWidget(parent),
     _caller(caller),
     ui(new Ui::PeakListPropertyWidget)
{
    ui->setupUi(this);
    std::map<std::string,std::shared_ptr<DataSet>>  datamap=_caller->getExperiment()->getData();
    std::vector<std::shared_ptr<SX::Data::DataSet>> datav;

    auto func = [&](std::pair<std::string,std::shared_ptr<SX::Data::DataSet>> value){datav.push_back(value.second);};

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
