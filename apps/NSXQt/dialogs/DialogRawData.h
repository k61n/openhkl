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

#ifndef DIALOGRAWDATA_H
#define DIALOGRAWDATA_H

#include <QDialog>

namespace SX {
namespace Instrument {
class Experiment;
}
}

namespace Ui {
class DialogRawData;
}

class DialogRawData : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRawData(QWidget *parent = 0);
    ~DialogRawData();

    double wavelength();
    double deltaChi();
    double deltaOmega();
    double deltaPhi();

    bool swapEndian();
    bool rowMajor();
    int bpp();

    void setWavelength(double new_wavelength);

private:
    Ui::DialogRawData *ui;
};

#endif // DIALOGRAWDATA_H
