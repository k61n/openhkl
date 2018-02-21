/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2016- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
          Jonathan Fisher, Forschungszentrum Juelich GmbH
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
    j.fisher[at]fz-juelich.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef NSXQT_PEAKTABLEVIEW_H
#define NSXQT_PEAKTABLEVIEW_H

#include <memory>
#include <string>

#include <QFileDialog>
#include <QMenu>
#include <QTableView>

#include <nsxlib/CrystalTypes.h>

#include <MainWindow.h>

class QContextMenuEvent;
class QMouseEvent;
class QString;
class QWidget;

class PeakCustomPlot;

class PeakTableView : public QTableView
{
    Q_OBJECT
public:

    explicit PeakTableView(QWidget* parent = 0);
    void contextMenuEvent(QContextMenuEvent *);

    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index, const QEvent *event) const;

    void keyPressEvent(QKeyEvent *event);

signals:
    void plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&);
    void plotPeak(nsx::sptrPeak3D);
    void autoIndexed();

public slots:
    //! Write the current list to FullProf
    void writeFullProf();
    //! Write the currrent list to ShelX
    void writeShelX();
    //! Normalize to monitor.
    void normalizeToMonitor();
    //! Plot as function of parameter. Needs to be a numeric type
    void plotAs(const std::string& key);
    //! Search peaks with hkl matching part of the string. Text must represent h,k,l values separated by white spaces
    void showPeaksMatchingText(const QString& text);
    //! Plot selected peak
    void plotSelectedPeak(const QModelIndex& index);

    void clearSelectedPeaks();
    void selectAllPeaks();
    void selectValidPeaks();
    void selectUnindexedPeaks();
    void togglePeaksSelection();

    void updateUnitCell(const nsx::sptrUnitCell& unitCell);

    void openAutoIndexingDialog();
    void openRefiningParametersDialog();
    void openProfileFitDialog();

private:

    static bool writeNewShelX(std::string filename, const nsx::PeakList& peaks);
    static bool writeStatistics(std::string filename, const nsx::PeakList& peaks,
                                double dmin, double dmax, int shells, bool friedel);

    void sortByHKL(bool up);
    void sortByIntensity(bool up);
    void sortByNumor(bool up);
    void sortBySelected(bool up);
    void sortByTransmission(bool up);
    bool checkBeforeWriting();
    void constructTable();

    bool checkBeforeWritting();

    std::string getPeaksRange() const;

private:

    //! Flag indicating that data have been normalized either to time or monitor.
    bool _normalized;
    bool _friedel;

};

#endif // NSXQT_PEAKTABLEVIEW_H
