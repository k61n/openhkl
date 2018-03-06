#pragma once

#include <QDialog>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/FitProfile.h>
#include <nsxlib/InstrumentTypes.h>

#include "ColorMap.h"

namespace Ui {
class ProfileFitDialog;
}

class DialogProfileFit: public QDialog
{
    Q_OBJECT

public:
    
    explicit DialogProfileFit(nsx::sptrExperiment experiment,
                                  nsx::sptrUnitCell unitCell,
                                  const nsx::PeakList& peaks,
                                  QWidget *parent = 0);
    ~DialogProfileFit();

    const nsx::FitProfile& profile();
    nsx::sptrShapeLibrary library() const;

private slots:    
    void calculate();
    void drawFrame(int value);


private:
    Ui::ProfileFitDialog *ui;
    nsx::sptrExperiment _experiment;
    nsx::sptrUnitCell _unitCell;
    nsx::PeakList _peaks;
    std::set<nsx::sptrDataSet> _data;
    nsx::FitProfile _profile;
    double _maximum;
    ColorMap _cmap;
    nsx::sptrShapeLibrary _library;
};
