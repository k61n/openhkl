#include "PeakListPropertyWidget.h"
#include "ui_PeakListPropertyWidget.h"
#include "PeakTableView.h"
#include "PeakListItem.h"
#include "ExperimentItem.h"
#include "SampleItem.h"
#include "InstrumentItem.h"
#include "IData.h"
#include "UnitCellItem.h"
#include "CollectedPeaksModel.h"
#include "CollectedPeaksProxyModel.h"

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


    CollectedPeaksModel* model = new CollectedPeaksModel();
    for (auto ptr : datav)
    {
        // Add peaks present in this numor to the LatticeFinder
        for (sptrPeak3D peak : ptr->getPeaks())
            model->addPeak(peak);
    }
    CollectedPeaksProxyModel* proxy = new CollectedPeaksProxyModel();
    proxy->setSourceModel(model);
    ui->tableView->setModel(proxy);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->sortByColumn(0);


//    ui->tableView->setData(datav);

    ui->selectedCells->setDefaultText("Selected cells");

    ExperimentItem* exptItem = dynamic_cast<ExperimentItem*>(_caller->parent());
    SampleItem* sampleItem = exptItem->getInstrumentItem()->getSampleItem();

    for (auto ucItem : sampleItem->getUnitCellItems())
        ui->selectedCells->addItem(ucItem->text());

    //Connect search box
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),ui->tableView,SLOT(showPeaksMatchingText(QString)));
    connect(ui->selectedCells,SIGNAL(checkedItemsChanged(QStringList)),this,SLOT(changeSelectedCells(QStringList)));
}

PeakListPropertyWidget::~PeakListPropertyWidget()
{
    delete ui;
}

PeakTableView* PeakListPropertyWidget::getPeakTableView() const
{
    return ui->tableView;
}

void PeakListPropertyWidget::changeSelectedCells(QStringList selectedCells)
{
    qDebug()<<selectedCells;
}

void PeakListPropertyWidget::on_obsPeaks_clicked(bool checked)
{
    qDebug()<<checked;
}
