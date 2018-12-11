#pragma once

#include <Eigen/Dense>

#include <QDialog>

#include <core/UnitCell.h>

namespace Ui {
class DialogTransformationMatrix;
}

class Widget;

class DialogTransformationMatrix : public QDialog {
    Q_OBJECT

signals:
    void getMatrix(const Eigen::Matrix3d& m);

public:
    explicit DialogTransformationMatrix(nsx::sptrUnitCell unitCell, QWidget* parent = 0);
    ~DialogTransformationMatrix();

public slots:
    virtual void accept() override;

private:
    Ui::DialogTransformationMatrix* ui;
    nsx::sptrUnitCell _unitCell;
};
