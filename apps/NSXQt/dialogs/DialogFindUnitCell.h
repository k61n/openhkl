#ifndef DIALOGFINDUNITCELL_H
#define DIALOGFINDUNITCELL_H

#include <QDialog>
#include <vector>
#include <memory>
#include "UnitCell.h"

namespace Ui {
class DialogFindUnitCell;
}

namespace SX
{

namespace Crystal
{
class Peak3D;
using sptrPeak3D=std::shared_ptr<Peak3D>;
}

namespace Instrument
{
class Experiment;
}

}

using SX::Crystal::UnitCell;
using SX::Crystal::sptrPeak3D;
using SX::Instrument::Experiment;

class DialogFindUnitCell : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFindUnitCell(std::shared_ptr<Experiment> experiment, QWidget *parent = 0);
    ~DialogFindUnitCell();
signals:
    void solutionAccepted(const SX::Crystal::UnitCell&);
private slots:
    void on_pushButton_SearchUnitCells_clicked();
    void setIOverSigma(int peaknumber);
    void buildSolutionsTable();
    void selectSolution(int);

//    void on_lineEdit_textChanged(const QString &arg1);

private:
    void setPeaks();
    Ui::DialogFindUnitCell *ui;
    std::shared_ptr<Experiment> _experiment;
    std::vector<sptrPeak3D> _peaks;
    std::vector<std::pair<UnitCell,double>> _solutions;
};

#endif // DIALOGFINDUNITCELL_H
