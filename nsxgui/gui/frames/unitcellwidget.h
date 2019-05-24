
#ifndef NSXGUI_GUI_FRAMES_UNITCELLWIDGET_H
#define NSXGUI_GUI_FRAMES_UNITCELLWIDGET_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include <core/CrystalTypes.h>

class UnitCellWidget : public QcrWidget {
    Q_OBJECT
public:
    UnitCellWidget(nsx::sptrUnitCell, const QString&);
    nsx::sptrUnitCell unitCell() const { return unitCell_; }

private:
    nsx::sptrUnitCell unitCell_;
};

#endif // NSXGUI_GUI_FRAMES_UNITCELLWIDGET_H
