#include "DialogCalculatedPeaks.h"
#include "ui_DialogCalculatedPeaks.h"

#include <nsxlib/UnitCell.h>

DialogCalculatedPeaks::DialogCalculatedPeaks(const std::set<nsx::sptrUnitCell>& cells, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogCalculatedPeaks),
    _cells(cells)    
{
    ui->setupUi(this);

    for (auto cell: _cells) {
        ui->comboBoxUC->addItem(cell->name().c_str());
    }
}

DialogCalculatedPeaks::~DialogCalculatedPeaks()
{
    delete ui;
}

double DialogCalculatedPeaks::dMin()
{
    return ui->dMinSpinBox->value();
}

double DialogCalculatedPeaks::dMax()
{
    return ui->dMaxSpinBox->value();
}

double DialogCalculatedPeaks::peakScale()
{
    return ui->peakScaleSpinBox->value();
}

double DialogCalculatedPeaks::bkgScale()
{
    return ui->bkgScaleSpinBox->value();
}

double DialogCalculatedPeaks::Isigma()
{
    return ui->sigmaSpinBox->value();
}

double DialogCalculatedPeaks::sigmaD()
{
    return ui->sigmaD->value();
}


double DialogCalculatedPeaks::sigmaM()
{
    return ui->sigmaM->value();
}

double DialogCalculatedPeaks::radius()
{
    return ui->radius->value();
}

double DialogCalculatedPeaks::nframes()
{
    return ui->nframes->value();
}

int DialogCalculatedPeaks::minimumNeighbors()
{
    return ui->neighborSpinBox->value();
}

nsx::sptrUnitCell DialogCalculatedPeaks::cell()
{
    for (auto cell: _cells) {
        if (ui->comboBoxUC->currentText() == cell->name().c_str()) {
            return cell;
        }
    }
    throw std::runtime_error("invalid unit cell encountered!");
}
