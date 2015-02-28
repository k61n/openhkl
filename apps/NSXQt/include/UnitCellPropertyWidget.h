#ifndef UnitCellPropertyWidget_H
#define UnitCellPropertyWidget_H

#include <QWidget>

namespace Ui {
class UnitCellPropertyWidget;
}

class UnitCellItem;

class UnitCellPropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UnitCellPropertyWidget(UnitCellItem* caller,QWidget *parent = 0);
    ~UnitCellPropertyWidget();

private slots:
    void setLatticeParams();

    void on_pushButton_Info_clicked();

private:
    UnitCellItem* _unitCellItem;
    Ui::UnitCellPropertyWidget *ui;
};

#endif // UnitCellPropertyWidget_H
