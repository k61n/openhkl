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

double DialogPredictPeaks::dMin()
{
    return ui->dMinSpinBox->value();
}

double DialogPredictPeaks::dMax()
{
    return ui->dMaxSpinBox->value();
}

double DialogPredictPeaks::radius()
{
    return ui->radius->value();
}

double DialogPredictPeaks::nFrames()
{
    return ui->nframes->value();
}

int DialogPredictPeaks::minNeighbors()
{
    return ui->neighborSpinBox->value();
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
