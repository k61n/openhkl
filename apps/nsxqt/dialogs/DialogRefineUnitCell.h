#ifndef NSXQT_DIALOGREFINEUNITCELL_H
#define NSXQT_DIALOGREFINEUNITCELL_H

#include <nsxlib/crystal/CrystalTypes.h>
#include <nsxlib/crystal/UBMinimizer.h>
#include <nsxlib/instrument/InstrumentTypes.h>

#include <QDialog>

namespace Ui
{
class DialogRefineUnitCell;
}

class DialogRefineUnitCell : public QDialog
{
    Q_OBJECT

public:
    
    explicit DialogRefineUnitCell(nsx::sptrExperiment experiment,
                                  nsx::sptrUnitCell unitCell,
                                  nsx::PeakList peaks,
                                  QWidget *parent = 0);
    ~DialogRefineUnitCell();
    void setLatticeParams();
    void setSampleOffsets();
    void setDetectorOffsets();
    void setWavelength();
    void setMinimizer();
    void setSolution(const nsx::UBSolution& solution);

signals:
    void cellUpdated(nsx::sptrUnitCell);

private slots:
    void refineParameter(bool checked, int i);
    void cellSampleHasChanged(int i, int j);
    void cellDetectorHasChanged(int i, int j);
    void refineParameters();
    void createOffsetsTables();
    void resetParameters();

private:
    Ui::DialogRefineUnitCell *ui;
    nsx::sptrExperiment _experiment;
    nsx::sptrUnitCell _unitCell;
    nsx::PeakList _peaks;
    nsx::UBMinimizer _minimizer;
};

#endif // NSXQT_DIALOGREFINEUNITCELL_H
