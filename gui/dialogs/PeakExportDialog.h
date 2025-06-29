//  ******************e*****************************************************************************k
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/dialogs/PeakExportDialog.h
//! @brief     Defines class PeakExportDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_DIALOG_PEAKEXPORTDIALOG
#define OHKL_GUI_DIALOG_PEAKEXPORTDIALOG

#include "core/statistics/PeakExporter.h"

#include <QDialog>

class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QRadioButton;
class QSpinBox;
class QTextEdit;

class DataComboBox;
class IntegratedPeakComboBox;

class PeakExportDialog : public QDialog {
    Q_OBJECT

 public:
    PeakExportDialog();
    //! loads parameters from PeakMerger to gui
    void loadMergeParams();
    //! Initialise dialogue from options in another GUI element
    void initialise(
        const QString& collection1, const QString& collection2, double d_min, double d_max,
        bool merged);
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
    bool useMergedPeakCollection() { return _merged_data; };

 public slots:
    void setSumIntensities(bool flag);

 private:
    //! Update parameters on change of data set
    void onDataChanged();
    //! Process the merge
    void processMerge();
    //! Do a single-batch refine to get a single unit cell for export
    ohkl::sptrUnitCell singleBatchRefine();

    //! For exporting peak lists
    ohkl::PeakExporter _exporter;

    QDialogButtonBox* _button_box;
    DataComboBox* _data_combo;
    IntegratedPeakComboBox* _peak_combo_1;
    IntegratedPeakComboBox* _peak_combo_2;
    QComboBox* _format_combo;
    QTextEdit* _textbox;
    QRadioButton* _rb_merged;
    QRadioButton* _rb_unmerged;
    QRadioButton* _rb_sum;
    QRadioButton* _rb_profile;

    std::string _selected_data;
    std::string _selected_pc;
    std::string _comment;
    bool _merged_data;

    QDoubleSpinBox* _drange_min;
    QDoubleSpinBox* _drange_max;
    QSpinBox* _frame_min;
    QSpinBox* _frame_max;
    QDoubleSpinBox* _scale_factor;
    QCheckBox* _friedel;
};

#endif // OHKL_GUI_DIALOG_PEAKEXPORTDIALOG
