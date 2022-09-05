//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/MtzExportDialog.cpp
//! @brief     Implements class MtzExportDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "MtzExportDialog.h" 

namespace ohkl {

MtzExportDialog::MtzExportDialog(bool merged_data) 
    : _selected_data(""), _selected_pc(""), _comment(""), _merged_data(merged_data)
{
    setModal(true);
    resize(600, 350);
    setMinimumSize(600, 350);
    setMaximumSize(600, 350);

    setWindowTitle(QString("Mtz Export"));

    QLabel* lab_data = new QLabel(QString("Dateset:"));
    QLabel* lab_peaks = new QLabel(QString("PeakCollection:"));
    QLabel* lab_rb_unmerged = new QLabel(QString("Unmerged data:"));
    QLabel* lab_rb_merged = new QLabel(QString("Merged data:"));
    QLabel* lab_note = new QLabel(QString("Note:"));

    QGridLayout* gridLayout = new QGridLayout(this);
   // GridFiller gfiller(gridLayout);

    _datacombo = new DataComboBox();
    _peakcombo = new QComboBox();
    _textbox = new QTextEdit();

    _rb_group = new QGroupBox();
    _rb_merged = new QRadioButton( );
    _rb_unmerged = new QRadioButton( );
    _rb_group->setTitle("Export");

    QHBoxLayout* hlay_cmb = new QHBoxLayout();
    QHBoxLayout* hlay_rb = new QHBoxLayout(_rb_group);
    QVBoxLayout* vlay = new QVBoxLayout();
    
    vlay->setSpacing(10);
    
    hlay_cmb->addWidget(lab_data);
    hlay_cmb->addWidget(_datacombo);
    hlay_cmb->addWidget(lab_peaks);
    hlay_cmb->addWidget(_peakcombo);

    hlay_rb->addWidget(lab_rb_unmerged);
    hlay_rb->addWidget(_rb_unmerged);
    hlay_rb->addWidget(lab_rb_merged);
    hlay_rb->addWidget(_rb_merged);
    
    vlay->addLayout(hlay_cmb);
    vlay->addWidget(_rb_group);
 
    vlay->addWidget(lab_note);
    vlay->addWidget(_textbox);
    gridLayout->addLayout(vlay,0,0);
    
    _button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    gridLayout->addWidget(_button_box);

    _rb_merged->setChecked(_merged_data);
    _rb_unmerged->setChecked(!_merged_data);

   // connect(_button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(_button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(_button_box, &QDialogButtonBox::accepted, this, 
    [=](){
       
        _selected_data = _datacombo->currentData()->name();
        _selected_pc = _peakcombo->currentText().toStdString();
        _comment = _textbox->toPlainText().toStdString();
        _merged_data = _rb_merged->isChecked();
        accept();
        }
    );
 
    refresh();
}

MtzExportDialog::~MtzExportDialog()
{}

void MtzExportDialog::refresh()
{ 
    _datacombo->refresh();

    auto pcollections = gSession->currentProject()->experiment()->getPeakCollections();

    for (auto & e : pcollections){
        if (e->type() == ohkl::PeakCollectionType::FOUND)
            continue;
        _peakcombo->addItem(QString::fromStdString(e->name()));
    }
}
}
