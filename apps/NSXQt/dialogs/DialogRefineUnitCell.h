#ifndef DIALOGREFINEUNITCELL_H
#define DIALOGREFINEUNITCELL_H

#include <QDialog>

#include <memory>
#include "UnitCell.h"
#include "UBMinimizer.h"

namespace Ui
{
class DialogRefineUnitCell;
}

namespace SX
{
namespace Instrument
{
class Experiment;
}
}

class DialogRefineUnitCell : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRefineUnitCell(std::shared_ptr<SX::Instrument::Experiment> experiment,
                                  std::shared_ptr<SX::Crystal::UnitCell> cell,
                                  QWidget *parent = 0);
    ~DialogRefineUnitCell();
    void setLatticeParams();
    void setSampleOffsets();
    void setDetectorOffsets();
    void setWavelength();
    void setMinimizer();
    void setSolution(const SX::Crystal::UBSolution& solution);
private slots:
    void refineParameter(bool checked,int i);
    void cellSampleHasChanged(int i,int j);
    void cellDetectoreHasChanged(int i,int j);
    void on_pushButton_Refine_clicked();
    void createOffsetsTables();
    void on_pushButton_Reset_clicked();

private:
    Ui::DialogRefineUnitCell *ui;
    std::shared_ptr<SX::Instrument::Experiment> _experiment;
    std::shared_ptr<SX::Crystal::UnitCell> _cell;
    SX::Crystal::UBMinimizer _minimizer;
};

#endif // DIALOGREFINEUNITCELL_H
