//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/ShapeLibraryDialog.h
//! @brief     Defines class ShapeLibraryDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_DIALOGS_SHAPELIBRARYDIALOG_H
#define GUI_DIALOGS_SHAPELIBRARYDIALOG_H

#include <QDialogButtonBox>
#include <QGraphicsView>
#include <QTableView>

#include "core/peak/Peak3D.h"
#include "core/experiment/DataTypes.h"
#include "core/shape/ShapeLibrary.h"
#include "core/shape/Profile3D.h"

#include "gui/models/ColorMap.h"
#include <QCR/widgets/actions.h>
#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <set>

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

#endif // GUI_DIALOGS_SHAPELIBRARYDIALOG_H
