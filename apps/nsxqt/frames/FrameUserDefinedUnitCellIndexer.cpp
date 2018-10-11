#include <nsxlib/Peak3D.h>

#include <ExperimentItem.h>
#include <PeaksItem.h>
#include <SessionModel.h>

#include "CollectedPeaksModel.h"
#include "FrameUserDefinedUnitCellIndexer.h"

#include "ui_FrameUserDefinedUnitCellIndexer.h"

FrameUserDefinedUnitCellIndexer* FrameUserDefinedUnitCellIndexer::_instance = nullptr;

FrameUserDefinedUnitCellIndexer* FrameUserDefinedUnitCellIndexer::create(ExperimentItem *experiment_item, const nsx::PeakList &peaks)
{
    if (!_instance) {
        _instance = new FrameUserDefinedUnitCellIndexer(experiment_item, peaks);
    }

    return _instance;
}

FrameUserDefinedUnitCellIndexer* FrameUserDefinedUnitCellIndexer::Instance()
{
    return _instance;
}

FrameUserDefinedUnitCellIndexer::FrameUserDefinedUnitCellIndexer(ExperimentItem* experiment_item, const nsx::PeakList &peaks)
: NSXQFrame(),
  _ui(new Ui::FrameUserDefinedUnitCellIndexer),
  _experiment_item(experiment_item)
{
    _ui->setupUi(this);

    CollectedPeaksModel *peaks_model = new CollectedPeaksModel(_experiment_item->model(),_experiment_item->experiment(),peaks);
    _ui->peaks->setModel(peaks_model);
    _ui->peaks->selectAll();

    _ui->label_a->setText("a ("+ QString(QChar(0x212B)) + ")");
    _ui->label_b->setText("b ("+ QString(QChar(0x212B)) + ")");
    _ui->label_c->setText("c ("+ QString(QChar(0x212B)) + ")");

    _ui->label_alpha->setText(QString(QChar(0x03B1)) + "(" + QString(QChar(0x00B0)) + ")");
    _ui->label_beta->setText(QString(QChar(0x03B2)) + "(" + QString(QChar(0x00B0)) + ")");
    _ui->label_gamma->setText(QString(QChar(0x03B3)) + "(" + QString(QChar(0x00B0)) + ")");

    connect(_ui->actions,SIGNAL(clicked(QAbstractButton*)),this,SLOT(slotActionClicked(QAbstractButton*)));
}

FrameUserDefinedUnitCellIndexer::~FrameUserDefinedUnitCellIndexer()
{
    delete _ui;

    if (_instance) {
        _instance = nullptr;
    }
}

void FrameUserDefinedUnitCellIndexer::slotActionClicked(QAbstractButton *button)
{
    auto button_role = _ui->actions->standardButton(button);

    switch(button_role)
    {
    case QDialogButtonBox::StandardButton::Apply: {
        index();
        break;
    }
    case QDialogButtonBox::StandardButton::Cancel: {
        close();
        break;
    }
    case QDialogButtonBox::StandardButton::Ok: {
        accept();
        break;
    }
    default: {
        return;
    }
    }
}

void FrameUserDefinedUnitCellIndexer::index()
{
}

void FrameUserDefinedUnitCellIndexer::accept()
{
    auto peaks_item = _experiment_item->peaksItem();

    emit _experiment_item->model()->itemChanged(peaks_item);

    close();
}
