//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/views/PeakTableView.h
//! @brief     Defines classes PeaksTableModel and PeaksTableView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_MODELS_PEAKTABLEVIEW_H
#define GUI_MODELS_PEAKTABLEVIEW_H

#include <QTableView>

class PeaksTableView : public QTableView {
    Q_OBJECT
 public:
    explicit PeaksTableView(QWidget* parent = 0);
    //  void contextMenuEvent(QContextMenuEvent*) override;
    //     virtual void keyPressEvent(QKeyEvent* event) override;
    //  signals:
    //     void plotData(const QVector<double>&, const QVector<double>&, const QVector<double>&);
    //     void plotPeak(nsx::sptrPeak3D);
    //     void autoIndexed();
    //     void updateShapeLibrary(nsx::sptrShapeLibrary);
    //  public slots:
    //     //! Normalize to monitor.
    //     void normalizeToMonitor();
    //     //! Plot as function of parameter. Needs to be a numeric type
    //     void plotAs(const std::string& key);
    //     void selectPeak(QModelIndex index);
    //     void clearSelectedPeaks() { clearSelection(); }
    //     void selectAllPeaks() { selectAll(); }
    //     void selectValidPeaks();
    //     void selectUnindexedPeaks();
    //     void togglePeaksSelection();
    //  private slots:
    //     void togglePeakSelection(QModelIndex index);

    //  private:
    //     void sortByHKL(bool up);
    //     void sortByIntensity(bool up);
    //     void sortByNumor(bool up);
    //     void sortBySelected(bool up);
    //     void sortByTransmission(bool up);
    //     void constructTable();
};

#endif // GUI_MODELS_PEAKSTABLE_H
