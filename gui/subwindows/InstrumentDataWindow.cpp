#include "InstrumentDataWindow.h"

#include <QTableWidget>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QStringList>
#include <QTextEdit>
#include <QPushButton> 
#include <QLabel>
#include <gui/models/Session.h>
#include <gui/models/Project.h>
#include <core/experiment/Experiment.h>
#include <core/instrument/InstrumentState.h>
#include <core/instrument/InstrumentState.h>

using InstrumentStateList = std::vector<nsx::InstrumentState>;


InstrumentDataWindow::InstrumentDataWindow(QWidget* parent)
: QDialog(parent)
{
    setModal(false); 
    QVBoxLayout* lay = new QVBoxLayout(this);
    QHBoxLayout* head_layout = new QHBoxLayout();

    QLabel* lab = new QLabel();
    lab->setText(QString("Instrument"));

    _instruments = new QComboBox();
    _instruments->setMaximumSize(QSize(300,50));

    head_layout->addWidget(lab);
    head_layout->addWidget(_instruments);

    lay->addLayout(head_layout);

    _table = new QTableWidget(0,2);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setHorizontalHeaderLabels(QStringList{"Key", "Value" });
    _table->resizeColumnsToContents();
    _table->verticalHeader()->setVisible(false);

    lay->addWidget(_table);
}             

void InstrumentDataWindow::refreshAll()
{ 
    auto prj = gSession->currentProject();
    auto expt = prj->experiment();

    auto data =  expt->getAllData();
    for (auto &e : data){       
        auto d = expt->getData(e->name());    
        auto iss = expt->getInstrumentStateSet(d);
        InstrumentStateList isl = iss->getInstrumentStateList();

        for (int i=0; i<isl.size(); ++i){
            auto diff = isl.at(i).diffractometer();
            auto src  = diff->source();
            //  auto nmc src->nMonochromators()
            auto mc = src.monochromators();
        }
    }
}