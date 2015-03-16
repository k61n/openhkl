#ifndef UnitCellPropertyWidget_H
#define UnitCellPropertyWidget_H

#include <QWidget>
#include <memory>

namespace Ui {
class UnitCellPropertyWidget;
}
namespace SX{
    namespace Crystal{
    class UnitCell;
    }
}

class UnitCellItem;

class UnitCellPropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UnitCellPropertyWidget(UnitCellItem* caller,QWidget *parent = 0);
    ~UnitCellPropertyWidget();

signals:
    void activateIndexingMode(std::shared_ptr<SX::Crystal::UnitCell>);
private slots:
    void getLatticeParams();
    void setLatticeParams();
    void on_pushButton_Info_clicked();
    void on_pushButton_Index_clicked();
    void on_pushButton_AutoIndexing_clicked();
    void on_pushButton_Refine_clicked();

public slots:
    void setCell(const SX::Crystal::UnitCell&);
private:
    UnitCellItem* _unitCellItem;
    Ui::UnitCellPropertyWidget *ui;
};

#endif // UnitCellPropertyWidget_H
