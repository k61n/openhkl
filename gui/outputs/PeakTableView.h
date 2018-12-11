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
#pragma once

#include <string>

#include <QFileDialog>
#include <QMenu>
#include <QTableView>

#include <core/PeakList.h>

#include <MainWindow.h>

class QContextMenuEvent;
class QMouseEvent;
class QWidget;

class PeakTableView : public QTableView {
    Q_OBJECT
public:
    explicit PeakTableView(QWidget* parent = 0);

    void contextMenuEvent(QContextMenuEvent*);

    virtual void keyPressEvent(QKeyEvent* event) override;

signals:
    void plotData(const QVector<double>&, const QVector<double>&, const QVector<double>&);
    void plotPeak(nsx::sptrPeak3D);
    void autoIndexed();
    void updateShapeLibrary(nsx::sptrShapeLibrary);

public slots:
    //! Normalize to monitor.
    void normalizeToMonitor();
    //! Plot as function of parameter. Needs to be a numeric type
    void plotAs(const std::string& key);

    void selectPeak(QModelIndex index);

    void clearSelectedPeaks();
    void selectAllPeaks();
    void selectValidPeaks();
    void selectUnindexedPeaks();

    void togglePeaksSelection();

private slots:
    void togglePeakSelection(QModelIndex index);

private:
    void sortByHKL(bool up);
    void sortByIntensity(bool up);
    void sortByNumor(bool up);
    void sortBySelected(bool up);
    void sortByTransmission(bool up);
    void constructTable();
};
