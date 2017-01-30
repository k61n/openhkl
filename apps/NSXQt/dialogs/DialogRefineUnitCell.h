#ifndef DIALOGREFINEUNITCELL_H
#define DIALOGREFINEUNITCELL_H

#include <QDialog>

#include <memory>

#include "UnitCell.h"
#include "UBMinimizer.h"
#include "Types.h"

namespace Ui
{
class DialogRefineUnitCell;
}

using SX::Crystal::sptrPeak3D;
using SX::Crystal::sptrUnitCell;
using SX::Instrument::sptrExperiment;

class DialogRefineUnitCell : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRefineUnitCell(std::shared_ptr<SX::Instrument::Experiment> experiment,
                                  sptrUnitCell unitCell,
                                  std::vector<sptrPeak3D> peaks,
                                  QWidget *parent = 0);
    ~DialogRefineUnitCell();
    void setLatticeParams();
    void setSampleOffsets();
    void setDetectorOffsets();
    void setWavelength();
    void setMinimizer();
    void setSolution(const SX::Crystal::UBSolution& solution);

signals:
    void cellUpdated(sptrUnitCell);

private slots:
    void refineParameter(bool checked, int i);
    void cellSampleHasChanged(int i, int j);
    void cellDetectorHasChanged(int i, int j);
    void refineParameters();
    void createOffsetsTables();
    void resetParameters();

private:
    Ui::DialogRefineUnitCell *ui;
    sptrExperiment _experiment;
    sptrUnitCell _unitCell;
    std::vector<sptrPeak3D> _peaks;
    SX::Crystal::UBMinimizer _minimizer;
};

#endif // DIALOGREFINEUNITCELL_H
