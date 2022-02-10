//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_home/SubframeHome.cpp
//! @brief     Implements class SubframeHome
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_home/SubframeHome.h"

#include "core/experiment/Experiment.h"
#include "core/shape/PeakCollection.h"
#include "gui/dialogs/ExperimentDialog.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/MainWin.h"
#include "tables/crystal/UnitCell.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSpacerItem>
#include <qtablewidget.h>

SubframeHome::SubframeHome()
{
    QVBoxLayout* main_layout = new QVBoxLayout(this);

    QSpacerItem* spacer_top = new QSpacerItem(10, 50, QSizePolicy::Minimum, QSizePolicy::Fixed);
    main_layout->addSpacerItem(spacer_top);

    QHBoxLayout* sub_layout = new QHBoxLayout();
    QSpacerItem* spacer_left = new QSpacerItem(50, 10, QSizePolicy::Fixed, QSizePolicy::Minimum);
    sub_layout->addSpacerItem(spacer_left);

    _setLeftLayout(sub_layout);
    _setRightLayout(sub_layout);

    // QSpacerItem* spacer_right =
    //     new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Minimum);
    // sub_layout->addSpacerItem(spacer_right);
    main_layout->addLayout(sub_layout);

    QSpacerItem* spacer_bot = new QSpacerItem(10, 50, QSizePolicy::Minimum, QSizePolicy::Fixed);
    main_layout->addSpacerItem(spacer_bot);

    readSettings();
    toggleUnsafeWidgets();
    _updateLastLoadedWidget();
}

void SubframeHome::_setLeftLayout(QHBoxLayout* main_layout)
{
    QVBoxLayout* left = new QVBoxLayout;
    QHBoxLayout* left_top = new QHBoxLayout();
    QVBoxLayout* left_bottom = new QVBoxLayout();
    QString tooltip;

    _new_exp = new QPushButton();
    _new_exp->setIcon(QIcon(":/images/create_new.svg"));
    _new_exp->setText("Create new experiment");
    _new_exp->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    connect(_new_exp, &QPushButton::clicked, this, &SubframeHome::createNew);

    _old_exp = new QPushButton();
    _old_exp->setIcon(QIcon(":/images/load_from_folder.svg"));
    _old_exp->setText("Load from file");
    _old_exp->setMinimumWidth(_new_exp->sizeHint().width());
    _old_exp->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    tooltip = "Load an existing experiment from a .nsx (HDF5) file";
    _old_exp->setToolTip(tooltip);
    connect(_old_exp, &QPushButton::clicked, this, &SubframeHome::loadFromFile);

    left_top->addWidget(_new_exp);
    left_top->addWidget(_old_exp);

    left->addLayout(left_top);

    _last_import_widget = new QListWidget(this);
    _last_import_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _last_import_widget->setStyleSheet("background-color: transparent;");
    connect(_last_import_widget, &QListWidget::itemClicked, this, &SubframeHome::_loadSelectedItem);

    left->addWidget(_last_import_widget);

    QSpacerItem* spacer_bottom =
        new QSpacerItem(10, 15, QSizePolicy::Minimum, QSizePolicy::Expanding);
    left->addSpacerItem(spacer_bottom);
  


    /*
     nsx::PeakCollection* pc = nullptr;
    std::string current_pc = _peak_combo->currentText().toStdString();
    if (current_pc.size() == 0)
        return false;
    pc = gSession->currentProject()->experiment()->getPeakCollection( current_pc );    
    if (pc == nullptr)    
        return false;  
    if (pc->isIndexed()) {      
        _pc_indexed->setCheckState(Qt::CheckState::Checked);       
    } else _pc_indexed->setCheckState(Qt::CheckState::Unchecked);    
    if ( pc->isIntegrated()) {
       _pc_integrated->setCheckState(Qt::CheckState::Checked)  ;    
    } else _pc_integrated->setCheckState(Qt::CheckState::Unchecked);    
    */
 
    

    _peak_collections_information_table = new QTableWidget(2,5);
    _peak_collections_information_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    _peak_collections_information_table->setItem(0, 0, new QTableWidgetItem("Name of PeakCollection"));
    _peak_collections_information_table->setItem(0, 1, new QTableWidgetItem("Peaks"));
    _peak_collections_information_table->setItem(0, 2, new QTableWidgetItem("Indexed"));
    _peak_collections_information_table->setItem(0, 3, new QTableWidgetItem("Integration"));
    _peak_collections_information_table->setItem(0, 4, new QTableWidgetItem("unit cells"));



    _peaks_information_table = new QTableWidget(2,5);
    _peaks_information_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _peaks_information_table->setItem(0, 0, new QTableWidgetItem("Peak"));
    _peaks_information_table->setItem(0, 0, new QTableWidgetItem("Unit Cell"));

    UpdatePeakInformationTable();

    
    
    left->addWidget(_peak_collections_information_table);  
    left->addWidget(_peaks_information_table);  

    connect(_peak_collections_information_table, &QTableWidget::itemClicked, this, &SubframeHome::UpdatePeakList);

    main_layout->addLayout(left);
}

void SubframeHome::_setRightLayout(QHBoxLayout* main_layout)
{
    QVBoxLayout* right = new QVBoxLayout;
    QString tooltip;

    _open_experiments_model = std::make_unique<ExperimentModel>();
    _open_experiments_view = new ExperimentTableView();
    _open_experiments_view->setModel(_open_experiments_model.get());
    connect(
        _open_experiments_view, &ExperimentTableView::doubleClicked, this,
        &SubframeHome::_switchCurrentExperiment);

    right->addWidget(_open_experiments_view);

    QHBoxLayout* right_bot = new QHBoxLayout();

    _save_current = new QPushButton();
    _save_current->setIcon(QIcon(":/images/save.svg"));
    _save_current->setText("Save current experiment");
    _save_current->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    tooltip = "Save current experiment to .nsx (HDF5) file";
    _save_current->setToolTip(tooltip);
    connect(_save_current, &QPushButton::clicked, this, &SubframeHome::saveCurrent);

    _save_all = new QPushButton();
    _save_all->setIcon(QIcon(":/images/save.svg"));
    _save_all->setText("Save all experiments");
    _save_all->setMinimumWidth(_save_current->sizeHint().width());
    _save_all->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    tooltip = "Save all experiments to .nsx (HDF5) files";
    _save_all->setToolTip(tooltip);

    right_bot->addWidget(_save_current);
    right_bot->addWidget(_save_all);

    right->addLayout(right_bot);

    main_layout->addLayout(right);

    _open_experiments_view->resizeColumnsToContents();
}

void SubframeHome::createNew()
{
    std::unique_ptr<ExperimentDialog> exp_dialog(new ExperimentDialog);
    exp_dialog->exec();

    if (exp_dialog->result()) {
        QString expr_nm = exp_dialog->experimentName();
        QString instr_nm = exp_dialog->instrumentName();

        std::unique_ptr<Project> project_ptr {gSession->createProject
                                              (expr_nm, instr_nm)};
        const bool success = gSession->addProject(std::move(project_ptr));

        if (success) {
            _open_experiments_model.reset();
            _open_experiments_model = std::make_unique<ExperimentModel>();
            _open_experiments_view->setModel(_open_experiments_model.get());
            toggleUnsafeWidgets();
        }
    }
}

void SubframeHome::loadFromFile()
{
    gGui->setReady(false);
    QSettings s;
    s.beginGroup("RecentDirectories");
    QString loadDirectory = s.value("experiment", QDir::homePath()).toString();

    QString file_path = QFileDialog::getOpenFileName(
        this, "Load the current experiment", loadDirectory, "NSXTool file (*.nsx)");

    if (file_path.isEmpty())
        return;

    QFileInfo info(file_path);
    s.setValue("experiment", info.absolutePath());

    try {
        gSession->loadExperimentFromFile(file_path);

        _open_experiments_model.reset();
        _open_experiments_model = std::make_unique<ExperimentModel>();
        _open_experiments_view->setModel(_open_experiments_model.get());
        _updateLastLoadedList(
            QString::fromStdString(gSession->currentProject()->experiment()->name()), file_path);
        toggleUnsafeWidgets();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
    
    
    UpdatePeakInformationTable();
    gGui->setReady(true);
}


void SubframeHome::saveCurrent()
{
    gGui->setReady(false);
    QSettings s;
    s.beginGroup("RecentDirectories");
    QString loadDirectory = s.value("experiment", QDir::homePath()).toString();

    try {
        QString file_path = QFileDialog::getSaveFileName(
            this, "Save the current experiment", loadDirectory, "NSXTool file (*.nsx)");

        if (file_path.isEmpty())
            return;

        QFileInfo info(file_path);
        s.setValue("experiment", info.absolutePath());

        gSession->currentProject()->saveToFile(file_path);
        _updateLastLoadedList(
            QString::fromStdString(gSession->currentProject()->experiment()->name()), file_path);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
    gGui->setReady(true);
}

void SubframeHome::saveAll() { }

void SubframeHome::_switchCurrentExperiment(const QModelIndex& index) const
{
    gSession->selectProject(index.row());
    emit _open_experiments_model->dataChanged(QModelIndex(), QModelIndex());
    
}

void SubframeHome::saveSettings() const
{
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("last_loaded");
    QSettings s;
    s.beginGroup("RecentFiles");
    s.setValue("last_loaded", QVariant::fromValue(_last_imports));
}

void SubframeHome::readSettings()
{
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("last_loaded");
    QSettings s;
    s.beginGroup("RecentFiles");
    _last_imports = s.value("last_loaded").value<QList<QStringList>>();
}

void SubframeHome::_updateLastLoadedList(QString name, QString file_path)
{
    QStringList temp = {name, file_path};
    if (_last_imports.empty())
        _last_imports.prepend(temp);
    else if (_last_imports[0][0] != name)
        _last_imports.prepend(temp);

    if (_last_imports.size() > 5)
        _last_imports.removeLast();

    _updateLastLoadedWidget();
    UpdatePeakInformationTable();
}

void SubframeHome::_updateLastLoadedWidget()
{
    _last_import_widget->blockSignals(true);
    _last_import_widget->clear();

    QList<QStringList>::iterator it;
    for (it = _last_imports.begin(); it != _last_imports.end(); ++it) {
        std::ostringstream oss;
        oss << (*it).at(0).toStdString() << " (" << (*it).at(1).toStdString() << ")";
        QString fullname = QString::fromStdString(oss.str());
        QListWidgetItem* item = new QListWidgetItem(QIcon(":/images/experimentIcon.png"), fullname);
        item->setData(100, (*it).at(1));
        _last_import_widget->addItem(item);
    }

    _last_import_widget->blockSignals(false);
}

void SubframeHome::_loadSelectedItem(QListWidgetItem* item)
{
    try {
        gSession->loadExperimentFromFile(item->data(100).toString());
        _open_experiments_model.reset();
        _open_experiments_model = std::make_unique<ExperimentModel>();
        _open_experiments_view->setModel(_open_experiments_model.get());
        _updateLastLoadedList(
            QString::fromStdString(gSession->currentProject()->experiment()->name()),
            item->data(100).toString());
        _open_experiments_view->resizeColumnsToContents();
        toggleUnsafeWidgets();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
}

void SubframeHome::toggleUnsafeWidgets()
{
    _save_all->setEnabled(true);
    _save_current->setEnabled(true);
    if (_open_experiments_model->rowCount() == 0) {
        _save_all->setEnabled(false);
        _save_current->setEnabled(false);
    }
}

void SubframeHome::UpdatePeakInformationTable()
{
  
  try{
   // if (gSession->currentProject() == nullptr) return;
   // if (gSession->currentProject()->experiment() == nullptr) return;
    
    std::vector<std::string> pcs_names = gSession->currentProject()->experiment()->getCollectionNames();     
    if (pcs_names.size() > 0){
        std::vector<std::string>::iterator it; 
        nsx::PeakCollection* pc;
        auto b2c = [](bool a) { return a == false ? "No" : "Yes"; };

        for (it = pcs_names.begin(); it != pcs_names.end(); it++){
            pc = gSession->currentProject()->experiment()->getPeakCollection( *it );
            short n = std::distance(pcs_names.begin(), it)+1;

            if (n >= _peak_collections_information_table->rowCount())
            {
                _peak_collections_information_table->insertRow(_peak_collections_information_table->rowCount());
            }
           
         
            _peak_collections_information_table->setItem(n, 0, new QTableWidgetItem(QString( (*it).c_str() )));
            _peak_collections_information_table->setItem(n, 1, new QTableWidgetItem(QString::number(pc->numberOfPeaks())));
            _peak_collections_information_table->setItem(n, 2, new QTableWidgetItem(b2c(pc->isIndexed())));
            _peak_collections_information_table->setItem(n, 3, new QTableWidgetItem(b2c(pc->isIntegrated())));
            _peak_collections_information_table->setItem(n, 4, new QTableWidgetItem("unit cells"));   
            
        }  
    }
  } catch (const std::exception& e){
   // QMessageBox::critical(this, "Error", QString(e.what()));
  }
}

void SubframeHome::UpdatePeakList()
{
    
    try {

        _peaks_information_table->setItem(0, 0, new QTableWidgetItem(QString("Is Enabled")));
        _peaks_information_table->setItem(0, 1, new QTableWidgetItem(QString("Is Predicted")));
        _peaks_information_table->setItem(0, 2, new QTableWidgetItem(QString("Was Caught")));
        _peaks_information_table->setItem(0, 3, new QTableWidgetItem(QString("Vol")));
        _peaks_information_table->setItem(0, 4, new QTableWidgetItem(QString("unit cell")));
         

        int row   = _peak_collections_information_table->currentRow();
        QString pc_name = _peak_collections_information_table->item(row,0)->text();
        auto pc = gSession->currentProject()->experiment()->getPeakCollection(pc_name.toStdString());
       
        //if (pc == nullptr) return;

        auto peak_list = pc->getPeakList();
      
        //if (peak_list == nullptr) return;
        auto b2s = [](bool a) { return a == false ? QString("No") : QString("Yes"); };
        std::vector<nsx::Peak3D>::iterator it; 
        for (auto it = peak_list.begin(); it != peak_list.end(); it++){
            short n = std::distance(peak_list.begin(), it)+1;
            //auto peak = *it;

            if (n >= _peaks_information_table->rowCount())
            {
                _peaks_information_table->insertRow(_peaks_information_table->rowCount());
            }
            
            _peaks_information_table->setItem(n, 0, new QTableWidgetItem( b2s((*it)->enabled()) ));
            _peaks_information_table->setItem(n, 1, new QTableWidgetItem( b2s((*it)->predicted()) ));
            _peaks_information_table->setItem(n, 2, new QTableWidgetItem( b2s((*it)->caughtByFilter()) ));
            _peaks_information_table->setItem(n, 3, new QTableWidgetItem( QString::number((*it)->shape().volume() )));
            
           // std::cout << n << std::endl;


            const nsx::UnitCell* unit_cell = nullptr;
            unit_cell = (*it)->unitCell();
            if (unit_cell == nullptr)
            {
                _peaks_information_table->setItem(n, 4, new QTableWidgetItem( QString("None") ));
            }
            else 
            {
                
                _peaks_information_table->setItem(n, 4, new QTableWidgetItem( QString::fromStdString((*it)->unitCell()->bravaisTypeSymbol()) ));
            }
            
           // auto e = (*it)->unitCell()->character().a;
            
        }
    }    catch (const std::exception& e){
        QMessageBox::critical(this, "Error", QString(e.what()));
    }

     

}
