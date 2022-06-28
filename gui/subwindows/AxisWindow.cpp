#include "AxisWindow.h"

AxisWindow::AxisWindow(QWidget* parent) : QDialog(parent)
{
    auto layout = new QVBoxLayout(this);
    auto header_layout = new QHBoxLayout(); 

    QLabel* label0 = new QLabel();
    label0->setText("Select data to view");
    _data_selector = new QComboBox();
    
    header_layout->addWidget(label0);
    header_layout->addWidget(_data_selector); 
    QLabel* label1 = new QLabel();
    label1->setText("Select axis to view");

    _axis_selector = new QComboBox();
    for (int i=0; i<3; i++)
        _axis_selector->addItem(QString::number(i));

    header_layout->addWidget(label1);
    header_layout->addWidget(_axis_selector);
    layout->addLayout(header_layout);

    auto axis_name_layout = new QHBoxLayout();

    QLabel* label2 = new QLabel();
    label2->setText("Axis Name: ");
    _axis_name = new QLineEdit();

    axis_name_layout->addWidget(label2);
    axis_name_layout->addWidget(_axis_name);
    layout->addLayout(axis_name_layout);

    
    auto vector_layout = new QHBoxLayout(this);
    QLabel* label3 = new QLabel(QString("Vector:          "));
    vector_layout->addWidget(label3);
    vector_layout->setSpacing(50);
    _x_axis_value = new QLineEdit();
    _y_axis_value = new QLineEdit();
    _z_axis_value = new QLineEdit();
    vector_layout->addWidget(_x_axis_value);
    vector_layout->setSpacing(3);
    vector_layout->addWidget(_y_axis_value);
    vector_layout->setSpacing(3);
    vector_layout->addWidget(_z_axis_value);
    vector_layout->addStretch();

    layout->addLayout(vector_layout); 
    _axis_clockwise = new QCheckBox();
    _axis_physical = new QCheckBox(); 

    auto boolean_flags = new QHBoxLayout();
    QLabel* label4 = new QLabel(QString("clockwise"));
    boolean_flags->addWidget(label4);
    boolean_flags->setSpacing(7);
    boolean_flags->addWidget(_axis_clockwise);
    boolean_flags->addStretch();
    layout->addLayout(boolean_flags);
    boolean_flags = new QHBoxLayout();
    
    QLabel* label5 = new QLabel(QString("physical"));
    boolean_flags->addWidget(label5);
    boolean_flags->setSpacing(20);
    boolean_flags->addWidget(_axis_physical); 
    boolean_flags->addStretch();
    layout->addLayout(boolean_flags);

    layout->addLayout(boolean_flags);

    setWindowTitle("Axis Information");
     
    layout->setSizeConstraint( QLayout::SetFixedSize );

    // read only modus
    _axis_name->setEnabled(false);      
    _x_axis_value->setEnabled(false);      
    _y_axis_value->setEnabled(false);      
    _z_axis_value->setEnabled(false);      
    _axis_clockwise->setEnabled(false);      
    _axis_physical->setEnabled(false);    

    connect( _axis_selector, qOverload<int>(&QComboBox::currentIndexChanged), this, &AxisWindow::refreshAll); 
}

void AxisWindow::refreshAll()
{ 
    if (gSession->hasProject()){         
        Project* prj = gSession->currentProject();
        auto expt = prj->experiment();
        auto allData = prj->allData();   
        auto N = prj->allData().size();
        int id; 
            
        if (_data_selector->count() > 0 ){//clear combobox -- needed both clear() and removeItem to be stable working
            _data_selector->clear();
            for (int i=0; i<=_axis_selector->count();i++)
                _data_selector->removeItem(i);
            _data_selector->setCurrentIndex(-1);
        }
        if (N > 0){            
            for (int i=0; i<N; i++){    
                _data_selector->addItem(QString::fromStdString(prj->getData(i)->name()));                         
            }
            id = _data_selector->currentIndex();
            if ((id == -1)||(id >= N)) id = 0;//selects dataset by selected row in table 
            auto axis_id = _axis_selector->currentIndex();
            auto dataset = prj->allData()[id];               

            _axis_name->setText(QString::fromStdString(dataset->diffractometer()->sample().gonio().axis(axis_id).name()) );
            _x_axis_value->setText(QString::number(dataset->diffractometer()->sample().gonio().axis(axis_id).axis()[0]));
            _y_axis_value->setText(QString::number(dataset->diffractometer()->sample().gonio().axis(axis_id).axis()[1]));
            _z_axis_value->setText(QString::number(dataset->diffractometer()->sample().gonio().axis(axis_id).axis()[2]));
            _axis_physical->setChecked(dataset->diffractometer()->sample().gonio().axis(axis_id).physical());
             
            }
        }   
}