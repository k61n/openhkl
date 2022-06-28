#include "PeaklistWindow.h"
#include <QVBoxLayout>

PeaklistWindow::PeaklistWindow(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("List of peaks");
    auto layout = new QVBoxLayout(this);
    _peak_properties = new PeakProperties();
    layout->addWidget(_peak_properties);
    resize(1400, 600); 
}

void PeaklistWindow::refreshAll()
{
    //
    /*
     if (gSession->hasProject()){         
        Project* prj = gSession->currentProject();
        auto expt = prj->experiment();
        auto allData = prj->allData();   
        auto N = prj->allData().size();
        int id; 
            
        if (_data_selector->count() > 0 ){//clear combobox -- needed both clear() and removeItem to be stable working
            _data_selector->clear();
            for (int i=0; i<=_data_selector->count();i++)
                _data_selector->removeItem(i);
            _data_selector->setCurrentIndex(-1);
        }
        if (N > 0){            
            for (int i=0; i<N; i++){    
                _data_selector->addItem(QString::fromStdString(prj->getData(i)->name()));                         
            }
            id = _data_selector->currentIndex();
            if ((id == -1)||(id >= N)) id = 0;//selects dataset by selected row in table 
             
            auto dataset = prj->allData()[id];  
            }
        }  */
    _peak_properties->refreshInput();
}