//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/MtzExportDialog.h
//! @brief     Defines class MtzExportDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef _MTZ_EXPORT_DIALOG
#define _MTZ_EXPORT_DIALOG

#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDir>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QWidget>
#include <QLayout>
#include <QRadioButton>
#include <QGroupBox>

#include <string>


#include "gui/utility/DataComboBox.h"
#include "gui/models/Session.h"
#include "gui/models/Project.h"
#include "core/experiment/Experiment.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/LinkedComboBox.h"
#include "gui/utility/PeakComboBox.h" 

namespace ohkl {

class MtzExportDialog : public QDialog { 

    public:
        MtzExportDialog(bool merged_data);
        ~MtzExportDialog();

        void refresh();

        std::string getDataset() {return _selected_data;}
        std::string getPeakCollection() {return _selected_pc;}
        std::string getComment() {return _comment;}
        bool useMergedData() {return _merged_data;};

        
    private:
        QDialogButtonBox* _button_box;
        DataComboBox* _datacombo;
        QComboBox* _peakcombo;
        QTextEdit* _textbox;
        QRadioButton* _rb_merged;
        QRadioButton* _rb_unmerged;
        QGroupBox* _rb_group;

        std::string _selected_data;
        std::string _selected_pc;
        std::string _comment;
        bool _merged_data;
};

} // namespace ohkl

#endif
