
#ifndef NSXGUI_GUI_DIALOGS_SHAPELIBRARY_H
#define NSXGUI_GUI_DIALOGS_SHAPELIBRARY_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QGraphicsView>
#include <QGroupBox>
#include <QTableView>

#include <core/CrystalTypes.h>
#include <core/DataTypes.h>
#include <core/InstrumentTypes.h>
#include <core/Profile3D.h>

#include "apps/ColorMap.h"
#include <QCR/widgets/actions.h>
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

class ShapeLibraryDialog : public QDialog {
    Q_OBJECT
public:
    ShapeLibraryDialog();
    const nsx::Profile3D& profile() { return _profile; }
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

#endif // NSXGUI_GUI_DIALOGS_SHAPELIBRARY_H
