//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/PeakModel.h
//! @brief     Defines classes PeaksTableModel and PeaksTableView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_MODELS_PEAKSTABLE_H
#define GUI_MODELS_PEAKSTABLE_H

#include "core/instrument/InstrumentTypes.h"
#include "core/peak/Peak3D.h"
#include "core/shape/ShapeLibrary.h"
#include <QCR/widgets/tables.h>

class PeaksTableModel : public QcrTableModel {
    Q_OBJECT
 public:
    enum Column { h, k, l, px, py, frame, intensity, sigmaIntensity, numor, unitCell, d, count };

    explicit PeaksTableModel(const QString& name, nsx::sptrExperiment experiment);
    PeaksTableModel(
        const QString& name, nsx::sptrExperiment experiment, const nsx::PeakList& peaks);
    ~PeaksTableModel() = default;

    int rowCount() const override { return _peaks.size(); }
    int columnCount() const override { return Column::count; }
    int highlighted() const override { return 0; } // unused
    void onHighlight(int) override {} // unused
    //! Removes the PeakList that was displayed
    void reset();
    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    //! Sets the PeakList that is to be displayed in the PeaksTable
    void setPeaks(const nsx::PeakList& peaks);
    //! Returns the currently displayed PeakList
    const nsx::PeakList& peaks() const { return _peaks; }
    //! Returns whether the selected peak is valid
    bool indexIsValid(const QModelIndex& index) const;
    void sort(int column, Qt::SortOrder order) override;
    //! Normalizes the peaks to monitor
    //! @param factor : the factor for the normalization
    void normalizeToMonitor(double factor);
    //! Returns all QModelIndexes that contain unindexed peaks
    QModelIndexList unindexedPeaks();
    //! Returns all QModelIndexes the contain the selected peaks
    QModelIndexList selectedPeaks();
    //! Returns the experiment to which the displayed peaks belong
    nsx::sptrExperiment experiment() { return _experiment; }
    void selectPeak(const QModelIndex& index);
    void togglePeakSelection(QModelIndex peak_index);
 public slots:
    void slotChangeEnabledPeak(nsx::sptrPeak3D peak);
    void slotChangeMaskedPeaks(const nsx::PeakList& peaks);
    void slotRemoveUnitCell(nsx::UnitCell* unit_cell);
    void setUnitCell(
        nsx::UnitCell* unitCell, QModelIndexList selectedPeaks = QModelIndexList());
    void sortEquivalents();
 signals:
    void signalSelectedPeakChanged(nsx::sptrPeak3D peak);
    void unitCellUpdated();

 private:
    nsx::sptrExperiment _experiment;
    nsx::PeakList _peaks;
};

#endif