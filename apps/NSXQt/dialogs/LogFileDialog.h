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


#ifndef LOGFILEDIALOG_H
#define LOGFILEDIALOG_H

#include <QDialog>

#include <iostream>

namespace Ui {
class LogFileDialog;
}

class LogFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogFileDialog(QWidget *parent = 0);
    ~LogFileDialog();

    std::string mergedFilename() const;
    std::string unmergedFilename() const;
    std::string statisticsFilename() const;

    bool writeMerged() const;
    bool writeUnmerged() const;
    bool writeStatistics() const;

    double dmin() const;
    double dmax() const;

    double numShells() const;

private slots:
    void on_statisticsBox_toggled(bool checked);
    void on_mergedBox_toggled(bool checked);
    void on_unmergedBox_toggled(bool checked);

private:
    Ui::LogFileDialog *ui;
};

#endif // LOGFILEDIALOG_H
