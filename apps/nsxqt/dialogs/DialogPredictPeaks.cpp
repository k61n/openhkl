#include "DialogPredictPeaks.h"
#include "ui_DialogPredictPeaks.h"

#include <nsxlib/UnitCell.h>

DialogPredictPeaks::DialogPredictPeaks(const std::set<nsx::sptrUnitCell>& cells, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogPredictPeaks),
    _cells(cells)    
{
    ui->setupUi(this);

    for (auto cell: _cells) {
        ui->comboBoxUC->addItem(cell->name().c_str());
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
    for (auto cell: _cells) {
        if (ui->comboBoxUC->currentText() == cell->name().c_str()) {
            return cell;
        }
    }
    throw std::runtime_error("invalid unit cell encountered!");
}
