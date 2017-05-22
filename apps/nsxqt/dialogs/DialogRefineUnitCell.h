#ifndef NSXQT_DIALOGREFINEUNITCELL_H
#define NSXQT_DIALOGREFINEUNITCELL_H

#include <memory>

#include <nsxlib/crystal/UBMinimizer.h>
#include <nsxlib/crystal/UnitCell.h>

#include <QDialog>

namespace Ui
{
class DialogRefineUnitCell;
}

namespace nsx {
class Experiment;
class Peak3D;
class UnitCell;
}

class DialogRefineUnitCell : public QDialog
{
    Q_OBJECT

public:

    using sptrExperiment = std::shared_ptr<nsx::Experiment>;
    using sptrPeak3D = std::shared_ptr<nsx::Peak3D>;
    using sptrUnitCell = std::shared_ptr<nsx::UnitCell>;
    
    explicit DialogRefineUnitCell(sptrExperiment experiment,
                                  sptrUnitCell unitCell,
                                  std::vector<sptrPeak3D> peaks,
                                  QWidget *parent = 0);
    ~DialogRefineUnitCell();
    void setLatticeParams();
    void setSampleOffsets();
    void setDetectorOffsets();
    void setWavelength();
    void setMinimizer();
    void setSolution(const nsx::UBSolution& solution);

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
    nsx::UBMinimizer _minimizer;
};

#endif // NSXQT_DIALOGREFINEUNITCELL_H
