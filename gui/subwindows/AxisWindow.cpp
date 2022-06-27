#include "AxisWindow.h"

AxisWindow::AxisWindow()
{

    auto layout = new QVBoxLayout(this);
    auto header_layout = new QHBoxLayout();

    QLabel* label = new QLabel();
    label->setText("Select axis to view");

    _axis_selector = new QComboBox();
    for (int i=0; i<3; i++)
        _axis_selector->addItem(QString::number(i));

    header_layout->addWidget(label);
    header_layout->addWidget(_axis_selector);

    QLabel* label2 = new QLabel();
    label2->setText("Axis Name: ");
    _axis_name = new QLineEdit();





     /*
        
        QRadioButton* _axis_clockwise;
        QRadioButton* _axis_physical;
        QLineEdit* _x_axis_value;
        QLineEdit* _y_axis_value;
        QLineEdit* _z_axis_value;

        */
     

}

void AxisWindow::refreshAll()
{



}