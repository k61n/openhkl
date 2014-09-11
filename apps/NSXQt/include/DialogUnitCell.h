#ifndef DIALOGUNITCELL_H
#define DIALOGUNITCELL_H
#include "ui_dialog_UnitCell.h"
#include <QDialog>
#include <vector>
#include <functional>
#include <memory>
#include <functional>
#include "Peak3D.h"
#include <vector>
#include <Eigen/Dense>
#include "Cluster.h"
#include "NiggliReduction.h"
#include "GruberReduction.h"
#include "Units.h"
#include <QMessageBox>
#include <DialogTransformationMatrix.h>

class DialogUnitCell : public QDialog
{
    Q_OBJECT
public:
    explicit DialogUnitCell(QWidget *parent = 0);
    void setPeaks(const std::vector<std::reference_wrapper<SX::Geometry::Peak3D>>& peaks);
    ~DialogUnitCell();
signals:

public slots:
    void getUnitCell();
    void setUpValues();
    void setTransformationMatrix();
    void reindexHKL();

private slots:

private:
    std::shared_ptr<SX::Geometry::Basis> _basis;
    Ui::DialogUnitCell* ui;
    std::vector<std::reference_wrapper<SX::Geometry::Peak3D>> _peaks;
};

#endif // DialogUnitCell_H
