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

class DialogUnitCell : public QDialog
{
    Q_OBJECT
public:
    explicit DialogUnitCell(QWidget *parent = 0):QDialog(parent),ui(new Ui::DialogUnitCell)
    {
        ui->setupUi(this);
        this->setModal(true);
        connect(ui->pushButtonFindUnitCell,SIGNAL(clicked()),this,SLOT(getUnitCell()));
        connect(ui->pushButtonFindReindexHKL,SIGNAL(clicked()),this,SLOT(reindexHKL()));

    }
    void setPeaks(const std::vector<std::reference_wrapper<SX::Geometry::Peak3D>>& peaks)
    {
        _peaks=peaks;
        ui->spinBoxNumberPeaks->setValue(peaks.size());
    }

    ~DialogUnitCell()
    {}
signals:

public slots:
    void getUnitCell()
    {
        SX::Geometry::LatticeFinder finder(0.02,0.01);
        for (SX::Geometry::Peak3D& peak : _peaks)
        {
            Eigen::Vector3d realQ=peak.getQ();
            finder.addPoint(realQ[0],realQ[1],realQ[2]);
        }
        finder.run(3.0);
        Eigen::Vector3d as,bs,cs;
        if (!finder.determineLattice(as,bs,cs,30))
            return;

        SX::Geometry::Basis b=SX::Geometry::Basis::fromReciprocalVectors(as,bs,cs);
        SX::Crystal::NiggliReduction n(b.getMetricTensor(),1e-3);
        Eigen::Matrix3d newg,P;
        n.reduce(newg,P);
        b.transform(P);
        SX::Geometry::Basis niggli=b;
        SX::Crystal::GruberReduction gr(b.getMetricTensor(),1.0);
        Eigen::Matrix3d Pprime;
        SX::Crystal::UnitCell::Centring type;
        gr.reduce(Pprime,type);
        b.transform(Pprime);
        std::shared_ptr<SX::Geometry::Basis> conventional(new SX::Geometry::Basis(b));
        ui->doubleSpinBoxa->setValue(conventional->gete1Norm());
        ui->doubleSpinBoxb->setValue(conventional->gete2Norm());
        ui->doubleSpinBoxc->setValue(conventional->gete3Norm());
        ui->doubleSpinBoxalpha->setValue(conventional->gete2e3Angle()/SX::Units::deg);
        ui->doubleSpinBoxbeta->setValue(conventional->gete1e3Angle()/SX::Units::deg);
        ui->doubleSpinBoxgamma->setValue(conventional->gete1e2Angle()/SX::Units::deg);
        _basis=conventional;
    }
    void reindexHKL()
    {
        int success=0;
        for (SX::Geometry::Peak3D& peak : _peaks)
        {
            if (peak.setBasis(_basis))
                success++;
        }
        QMessageBox::information(this,"Indexation","Successfully indexed"+QString::number(success)+" peaks out of "+QString::number(_peaks.size()));
    }

private slots:


private:
    std::shared_ptr<SX::Geometry::Basis> _basis;
    Ui::DialogUnitCell* ui;
    std::vector<std::reference_wrapper<SX::Geometry::Peak3D>> _peaks;
};

#endif // DialogUnitCell_H
