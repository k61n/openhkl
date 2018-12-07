#pragma once

#include <QDialog>

#include <core/PeakList.h>
#include <core/DataTypes.h>
#include <core/InstrumentTypes.h>
#include <core/Profile3D.h>

#include "ColorMap.h"
#include "ExperimentItem.h"

namespace Ui {
class DialogShapeLibrary;
}

class ExperimentItem;

class DialogShapeLibrary: public QDialog
{
    Q_OBJECT

public:
    
    explicit DialogShapeLibrary(ExperimentItem* experiment_item,
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
    void selectTargetPeak(int row);

private:
    Ui::DialogShapeLibrary *ui;
    nsx::sptrUnitCell _unitCell;
    nsx::PeakList _peaks;
    std::set<nsx::sptrDataSet> _data;
    nsx::Profile3D _profile;
    double _maximum;
    ColorMap _cmap;
    nsx::sptrShapeLibrary _library;
};
