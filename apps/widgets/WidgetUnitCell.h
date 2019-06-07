#pragma once

#include <QWidget>

#include <core/CrystalTypes.h>

namespace Ui {
class WidgetUnitCell;
}

class WidgetUnitCell : public QWidget {

    Q_OBJECT

public:
    WidgetUnitCell(nsx::sptrUnitCell);

    ~WidgetUnitCell();

    nsx::sptrUnitCell unitCell() const;

private:
    Ui::WidgetUnitCell* _ui;

    nsx::sptrUnitCell _unit_cell;
};
