#pragma once

#include <QDialog>

#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataTypes.h>
#include <nsxlib/InstrumentTypes.h>
#include <nsxlib/Profile3D.h>

#include "ColorMap.h"

namespace Ui {
class DialogShapeLibrary;
}

class DialogShapeLibrary: public QDialog
{
    Q_OBJECT

public:
    
    explicit DialogShapeLibrary(nsx::sptrExperiment experiment,
                                nsx::sptrUnitCell unitCell,
                                const nsx::PeakList& peaks,
                                QWidget *parent = 0);
    ~DialogShapeLibrary();

    const nsx::Profile3D& profile();
    nsx::sptrShapeLibrary library() const;

private slots:    
    void calculate();
    void build();
    void drawFrame(int value);


private:
    Ui::DialogShapeLibrary *ui;
    nsx::sptrExperiment _experiment;
    nsx::sptrUnitCell _unitCell;
    nsx::PeakList _peaks;
    std::set<nsx::sptrDataSet> _data;
    nsx::Profile3D _profile;
    double _maximum;
    ColorMap _cmap;
    nsx::sptrShapeLibrary _library;
};
