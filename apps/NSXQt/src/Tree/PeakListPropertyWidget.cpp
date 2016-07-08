#include "include/Tree/PeakListPropertyWidget.h"
#include "ui_PeakListPropertyWidget.h"
#include "include/PeakTableView.h"
#include "include/Tree/PeakListItem.h"
#include "IData.h"
#include <QtDebug>

#include <memory>

PeakListPropertyWidget::PeakListPropertyWidget(PeakListItem* caller, QWidget *parent) :
     QWidget(parent),
     _caller(caller),
     ui(new Ui::PeakListPropertyWidget)
{
    ui->setupUi(this);
    std::map<std::string,std::shared_ptr<IData>>  datamap=_caller->getExperiment()->getData();
    std::vector<std::shared_ptr<SX::Data::IData>> datav;

    auto func = [&](std::pair<std::string,std::shared_ptr<SX::Data::IData>> value)
    {
        datav.push_back(value.second);
    };

    std::for_each(datamap.begin(), datamap.end(), func);

    ui->tableView->setData(datav);

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

