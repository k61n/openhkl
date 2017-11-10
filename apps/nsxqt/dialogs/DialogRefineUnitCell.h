#ifndef NSXQT_DIALOGREFINEUNITCELL_H
#define NSXQT_DIALOGREFINEUNITCELL_H

#include <QDialog>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/InstrumentTypes.h>
#include <nsxlib/UBMinimizer.h>
#include <nsxlib/UBSolution.h>

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

private slots:    
    void refineParameters();
    void resetParameters();

private:
    void createOffsetTables();
    void updateParameters();
    void updatePlot();

    nsx::UBSolution _initialValues;
    nsx::UBSolution _currentValues;
    Ui::DialogRefineUnitCell *ui;
    nsx::sptrExperiment _experiment;
    nsx::sptrUnitCell _unitCell;
    nsx::PeakList _peaks;
};

#endif // NSXQT_DIALOGREFINEUNITCELL_H
