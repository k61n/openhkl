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

#include "dialogs/FriedelDialog.h"
#include "ui_FriedelDialog.h"

#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/logger/Logger.h>

FriedelDialog::FriedelDialog(const std::vector<nsx::Peak3D*>& peaks, QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::FriedelDialog),
    _peaks(peaks)
{
    _ui->setupUi(this);
    findFriedelPairs();
}

FriedelDialog::~FriedelDialog()
{
    delete _ui;
}

void FriedelDialog::findFriedelPairs()
{
    _friedelPairs.clear();

    size_t size = _peaks.size();

    for (size_t i = 0; i < size; ++i) {
        Eigen::RowVector3i hkl1 = _peaks[i]->getIntegerMillerIndices();

        for (size_t j = i+1; j < size; ++j) {
            Eigen::RowVector3i hkl2 = _peaks[j]->getIntegerMillerIndices();
            // Friedel condition
            if ( hkl1 == -hkl2) {
                _friedelPairs.push_back(std::make_pair(_peaks[i], _peaks[j]));
            }
        }
    }

    double percent = 2.0 * _friedelPairs.size() * 100.0 / _peaks.size();
    nsx::info() << "Found " << _friedelPairs.size() << " Friedel pairs which accounts for " << percent << " percent of the peaks.";
}

void FriedelDialog::on_goodPairsButton_clicked()
{
    int count = 0;
    double threshold = _ui->thresholdSpinBox->value();

    // deselect ALL peaks
    for (nsx::Peak3D* peak: _peaks)
        peak->setSelected(false);

    for (size_t i = 0; i < _friedelPairs.size(); ++i) {
        nsx::Peak3D* a = _friedelPairs[i].first;
        nsx::Peak3D* b = _friedelPairs[i].second;

        // skip if masked
        if (a->isMasked() || b->isMasked())
            continue;

        double int_a = a->getScaledIntensity().getValue();
        double int_b = b->getScaledIntensity().getValue();

        if ( 2.0 * std::fabs(int_a-int_b) / (int_a+int_b) < threshold) {
            ++count;
            a->setSelected(true);
            b->setSelected(true);

        }
    }

    nsx::info() << "Selected " << count * 100.0 / _friedelPairs.size() << " good Friedel pairs.";
}
