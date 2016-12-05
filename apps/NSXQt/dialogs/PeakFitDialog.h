/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forshungszentrum Juelich GmbH
 52425 Juelich
 Germany
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

#ifndef PEAKFITDIALOG_H
#define PEAKFITDIALOG_H

#include <QDialog>
#include <memory>
#include "Peak3D.h"
#include "PeakFit.h"

class QGraphicsScene;

class SessionModel;

namespace Ui {
class PeakFitDialog;
}

class PeakFitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PeakFitDialog(SessionModel* session, QWidget *parent = 0);
    ~PeakFitDialog();

    bool changePeak();
    void updatePlots();

private slots:
    void on_hSpinBox_valueChanged(int arg1);
    void on_kSpinBox_valueChanged(int arg1);
    void on_lSpinBox_valueChanged(int arg1);

private:
    Ui::PeakFitDialog *ui;
    SessionModel* _session;
    SX::Crystal::sptrPeak3D _peak;
    std::unique_ptr<SX::Crystal::PeakFit> _peakFit;

    QGraphicsScene* _peakScene;
    QGraphicsScene* _fitScene;
    //QGraphicsScene* _
};

#endif // PEAKFITDIALOG_H
