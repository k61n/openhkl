#include "include/Tree/PeakListPropertyWidget.h"
#include "ui_PeakListPropertyWidget.h"
#include "include/PeakTableView.h"
#include "include/Tree/PeakListItem.h"
#include "IData.h"
PeakListPropertyWidget::PeakListPropertyWidget(PeakListItem* caller, QWidget *parent) :
     QWidget(parent),
     _caller(caller),
     ui(new Ui::PeakListPropertyWidget)
{
    ui->setupUi(this);
    const auto&  datamap=_caller->getExperiment()->getData();
    std::vector<SX::Data::IData*> datav;
    std::for_each(datamap.begin(),datamap.end(),[&](std::pair<std::string,SX::Data::IData*> value){datav.push_back(value.second);});
    ui->tableView->setData(datav);
}

PeakListPropertyWidget::~PeakListPropertyWidget()
{
    delete ui;
}

PeakTableView* PeakListPropertyWidget::getPeakTableView() const
{
    return ui->tableView;
}
