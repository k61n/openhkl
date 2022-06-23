//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/InputFilesWindows.cpp
//! @brief     Implements class InputFilesWindows
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subwindows/InputFilesWindow.h"

 
#include "gui/MainWin.h" // gGui
#include "base/utils/StringIO.h" // split
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "core/experiment/Experiment.h"

#include <QTableWidget>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QStringList>
#include <QTextEdit>
#include <QPushButton> 
#include <iostream>

#include <regex>
#include <vector>
 

InputFilesWindow::InputFilesWindow( QWidget* parent)
{
    setModal(false); 

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* label = new QLabel("Used input files");
    QPushButton* button = new QPushButton();


    _files_table = new QTableWidget();
    _files_table->setColumnCount(2);
    _files_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _files_table->setHorizontalHeaderLabels(QStringList{
    "Id", "Path"});
    _files_table->resizeColumnsToContents();
    _files_table->verticalHeader()->setVisible(false);

     layout->addWidget(label);
     layout->addWidget(_files_table);

     resize(800, 600);
 
} 

void InputFilesWindow::refreshAll(int id)
{
    
    if (gSession->hasProject()){   
       
        Project* prj = gSession->currentProject();
        auto expt = prj->experiment();
        
        auto N = prj->allData().size();
        if (N == 0) return;
        if ((id == -1)||(id >= N)) id = 0;
         
        //adding dropdown for further selection
        
        nsx::sptrDataSet data = prj->getData(id);
        
        const nsx::MetaData& metadata = data->metadata();
        const nsx::MetaDataMap& map = metadata.map(); 

        for (auto element : map)  
        {    
            if (element.first == "sources"){                     
                auto input = std::get<std::string>(element.second);
                    
                std::regex re("[\\|,:]");
                //the '-1' is what makes the regex split (-1 := what was not matched)
                std::sregex_token_iterator first{input.begin(), input.end(), re, -1}, last;
                std::vector<std::string> tokens{first, last};

                for (auto &e : tokens){                      
                    _files_table->insertRow ( _files_table->rowCount() ); 
                    _files_table->setItem   ( _files_table->rowCount()-1, 
                        0, new QTableWidgetItem(QString::number(_files_table->rowCount()-1))); 
                    _files_table->setItem   ( _files_table->rowCount()-1, 
                        1, new QTableWidgetItem(QString::fromStdString(e)));
                }
            }          
        }
    }
    _files_table->resizeColumnsToContents();
}

 
 /*
QSize PeakWindow::sizeHint() const
{
    double w = gGui->sizeHint().width();
    double h = QDialog::sizeHint().height();
    return QSize(w, h);
}*/
 