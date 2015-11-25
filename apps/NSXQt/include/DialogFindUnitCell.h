#ifndef DIALOGFINDUNITCELL_H
#define DIALOGFINDUNITCELL_H

#include <QDialog>
#include <vector>
#include <memory>
#include "UnitCell.h"

namespace Ui {
class DialogFindUnitCell;
}

namespace SX{
    namespace Crystal
    {
        class Peak3D;
    }
    namespace Instrument
    {
        class Experiment;
    }
}
class DialogFindUnitCell : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFindUnitCell(SX::Instrument::Experiment* experiment, QWidget *parent = 0);
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
    SX::Instrument::Experiment* _experiment;
    std::vector<SX::Crystal::Peak3D*> _peaks;
    std::vector<std::pair<SX::Crystal::UnitCell,double>> _solutions;
};

#endif // DIALOGFINDUNITCELL_H
