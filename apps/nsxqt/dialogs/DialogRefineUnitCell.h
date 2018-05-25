#pragma once

#include <QDialog>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/InstrumentTypes.h>

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

private:
    Ui::DialogRefineUnitCell *ui;
    nsx::sptrExperiment _experiment;
    nsx::sptrUnitCell _unitCell;
    nsx::PeakList _peaks;
    std::set<nsx::sptrDataSet> _data;
};
