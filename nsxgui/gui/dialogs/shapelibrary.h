
#ifndef SHAPELIBRARY_H
#define SHAPELIBRARY_H

#include <QDialog>
#include <QGraphicsView>
#include <QTableView>
#include <QDialogButtonBox>
#include <QGroupBox>

#include <core/CrystalTypes.h>
#include <core/DataTypes.h>
#include <core/InstrumentTypes.h>
#include <core/Profile3D.h>

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/qcr/widgets/controls.h"
#include "nsxgui/qcr/widgets/actions.h"
#include "apps/ColorMap.h"

class ShapeLibraryDialog : public QDialog {
    Q_OBJECT
public:
    ShapeLibraryDialog();
    const nsx::Profile3D &profile() { return _profile; }
    nsx::sptrShapeLibrary library() const { return _library; }
private:
    void layout();
    void calculate();
    void build();
    void drawframe(int value);
    void selectTargetPeak(int row);

    nsx::sptrShapeLibrary _library;
    nsx::sptrUnitCell _unitCell;
    nsx::PeakList _peaks;
    std::set<nsx::sptrDataSet> _data;
    nsx::Profile3D _profile;
    double _maximum;
    ColorMap _cmap;

    QcrTabWidget* tabs;
    QcrWidget* libraryTab;
    QcrSpinBox* nx;
    QcrSpinBox* ny;
    QcrSpinBox* nz;
    QGroupBox* kabsch;
    QcrDoubleSpinBox* sigmaD;
    QcrDoubleSpinBox* sigmaM;
    QcrDoubleSpinBox* minISigma;
    QcrDoubleSpinBox* minD;
    QcrDoubleSpinBox* maxD;
    QcrDoubleSpinBox* peakscale;
    QcrDoubleSpinBox* backgroundbegin;
    QcrDoubleSpinBox* backgroundend;
    QcrTextTriggerButton* buildShapeLibrary;
    QcrWidget* previewTab;
    QTableView* table;
    QcrDoubleSpinBox* x;
    QcrDoubleSpinBox* y;
    QcrDoubleSpinBox* frame;
    QcrDoubleSpinBox* radius;
    QcrDoubleSpinBox* nframes;
    QcrTextTriggerButton* calculateMeanProfile;
    QGraphicsView* graphics;
    QSlider* drawFrame;
    QDialogButtonBox* buttons;
};

#endif //SHAPELIBRARY_H
