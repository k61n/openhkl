#pragma once

#include <memory>

#include <QWidget>

#include <core/PeakList.h>

namespace Ui {
class UnitCellPropertyWidget;
}

class QStandardItem;
class QString;

class UnitCellItem;

class UnitCellPropertyWidget : public QWidget {
    Q_OBJECT

public:
    explicit UnitCellPropertyWidget(UnitCellItem* caller, QWidget* parent = 0);
    ~UnitCellPropertyWidget();

private slots:

    void setUnitCellName();

    void getLatticeParams();

    void setLatticeParams();

    void setChemicalFormula();

    void setSpaceGroup(QString sg);

    void setZValue(int z);

    void setMassDensity() const;

    void activateSpaceGroupCompletion(QString sg);

    void setIndexingTolerance(double);

    void update(QStandardItem* item = nullptr);

private:
    void updateCellParameters();

private:
    UnitCellItem* _unitCellItem;

    Ui::UnitCellPropertyWidget* ui;
};
