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

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QRadioButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <string>


#include "core/experiment/Experiment.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/LinkedComboBox.h"
#include "gui/utility/PeakComboBox.h"

namespace ohkl {

class MtzExportDialog : public QDialog {

 public:
    //! Constructor
    MtzExportDialog(bool merged_data);
    //! Destructor
    ~MtzExportDialog();
    //! loads parameters from PeaakMerger to gui
    void loadMergeParams();
    //! sets changed parameters from gui to PeakMerger
    void setMergeParams();
    //! Refresh gui
    void refresh();
    //! get selected Dataset (for later)
    std::string getDataset() { return _selected_data; }
    //! get selected PeakCollection name
    std::string getPeakCollection() { return _selected_pc; }
    //! get entered comment
    std::string getComment() { return _comment; }
    //! get if unmerged/merged data should be exported
    bool useMergedData() { return _merged_data; };

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

    QGroupBox* _comment_grp_box;
    QGroupBox* _merge_param_box;
    QDoubleSpinBox* _drange_min;
    QDoubleSpinBox* _drange_max;
    QDoubleSpinBox* _frame_min;
    QDoubleSpinBox* _frame_max;
    QDoubleSpinBox* _shell_res;
    QCheckBox* _friedel;
};

} // namespace ohkl

#endif
