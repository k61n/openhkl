/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Eric Pellegrini, Jonathan Fisher
 Institut Laue-Langevin
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

#ifndef NSXQT_FRIEDELDIALOG_H
#define NSXQT_FRIEDELDIALOG_H

#include <vector>

#include <QDialog>

#include <nsxlib/CrystalTypes.h>

class Qwidget;

namespace Ui {
class FriedelDialog;
}

class FriedelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FriedelDialog(const std::vector<nsx::Peak3D*>& peaks, QWidget *parent = 0);
    ~FriedelDialog();

    void findFriedelPairs();

private slots:
    void on_goodPairsButton_clicked();

private:
    Ui::FriedelDialog *_ui;
    std::vector<nsx::Peak3D*> _peaks;
    std::vector<std::pair<nsx::Peak3D*, nsx::Peak3D*>> _friedelPairs;
};

#endif // NSXQT_FRIEDELDIALOG_H
