#include "DialogPredictPeaks.h"
#include "ui_DialogPredictPeaks.h"

#include <nsxlib/UnitCell.h>

DialogPredictPeaks::DialogPredictPeaks(const nsx::UnitCellList& cells, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogPredictPeaks),
    _cells(cells)    
{
    ui->setupUi(this);

    for (auto cell: _cells) {
        ui->unitCells->addItem(cell->name().c_str());
    }
}

DialogPredictPeaks::~DialogPredictPeaks()
{
    delete ui;
}

double DialogPredictPeaks::dMin() const
{
    return ui->dMinSpinBox->value();
}

double DialogPredictPeaks::dMax() const
{
    return ui->dMaxSpinBox->value();
}

double DialogPredictPeaks::radius() const
{
    return ui->radius->value();
}

double DialogPredictPeaks::nFrames() const
{
    return ui->nframes->value();
}

int DialogPredictPeaks::minNeighbors() const
{
    return ui->neighborSpinBox->value();
}

int DialogPredictPeaks::interpolation() const
{
    return ui->interpolation->currentIndex();
}

nsx::sptrUnitCell DialogPredictPeaks::cell()
{
    return _cells[ui->unitCells->currentIndex()];
}
