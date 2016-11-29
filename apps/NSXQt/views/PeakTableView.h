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


#ifndef PEAKTABLEVIEW_H
#define PEAKTABLEVIEW_H

#include <map>
#include <string>
#include <tuple>
#include <memory>

#include <QFileDialog>
#include <QMenu>
#include <QTableView>
#include <IData.h>
#include <MainWindow.h>

class PeakCustomPlot;
class QContextMenuEvent;
class QMouseEvent;

namespace SX
{
namespace Crystal
{
class Peak3D;
using sptrPeak3D=std::shared_ptr<Peak3D>;
}
}

using SX::Crystal::sptrPeak3D;

class PeakTableView : public QTableView
{
    Q_OBJECT
public:
    explicit PeakTableView(QWidget* parent = 0);
    void setData(std::vector<std::shared_ptr<SX::Data::IData>>);
    void contextMenuEvent(QContextMenuEvent *);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
signals:
    void plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&);
    void plotPeak(sptrPeak3D);
public slots:
    //! Selection of peak is changed
    void peakChanged(QModelIndex current,QModelIndex last);
    //! Display context menu
//    void customMenuRequested(QPoint pos);
    //! Sort the current table by column inde
    void sortByColumn(int column);
    //! Sort equivalent reflections
    void sortEquivalents();
    //! Write the current list to FullProf
    void writeFullProf();
    //! Write the currrent list to ShelX
    void writeShelX();
    //! Write a detailed log to file
    void writeLog();
    //! Normalize to monitor.
    void normalizeToMonitor();
    //! Change selected status of peak when double-clicked
    void deselectPeak(QModelIndex index);
    //! Plot as function of parameter. Needs to be a numeric type
    void plotAs(const std::string& key);
    //! Search peaks with hkl matching part of the string. Text must represent h,k,l values separated by white spaces
    void showPeaksMatchingText(QString text);
private:
    void sortByHKL(bool up);
    void sortByIntensity(bool up);
    void sortByNumor(bool up);
    void sortBySelected(bool up);
    void sortByTransmission(bool up);
    bool checkBeforeWriting();
    void constructTable();
    std::string getPeaksRange() const;
    std::vector<sptrPeak3D> _peaks;
    //! Which column is sorted and up or down
    std::tuple<int,bool> _columnUp;
    PeakCustomPlot* _plotter;
    //! Flag indicating that data have been normalized either to time or monitor.
    bool _normalized;

};

#endif // PEAKTABLEVIEW_H
