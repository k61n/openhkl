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

#include <QFileDialog>

#include "dialogs/LogFileDialog.h"
#include "ui_LogFileDialog.h"


LogFileDialog::LogFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogFileDialog)
{
    ui->setupUi(this);
}

LogFileDialog::~LogFileDialog()
{
    delete ui;
}

std::string LogFileDialog::mergedFilename() const
{
    return ui->mergedEdit->text().toStdString();
}

std::string LogFileDialog::unmergedFilename() const
{
    return ui->unmergedEdit->text().toStdString();
}

std::string LogFileDialog::statisticsFilename() const
{
    return ui->statisticsEdit->text().toStdString();
}

bool LogFileDialog::writeMerged() const
{
    return ui->mergedBox->isChecked();
}

bool LogFileDialog::writeUnmerged() const
{
    return ui->unmergedBox->isChecked();
}

bool LogFileDialog::writeStatistics() const
{
    return ui->statisticsBox->isChecked();
}

double LogFileDialog::dmin() const
{
    return ui->dMinBox->value();
}

double LogFileDialog::dmax() const
{
    return ui->dMaxBox->value();
}

double LogFileDialog::numShells() const
{
    return ui->shellBox->value();
}

void LogFileDialog::on_statisticsBox_toggled(bool checked)
{
    if (!checked) {
        ui->statisticsEdit->setText("");
        return;
    }

    QString filename = QFileDialog::getSaveFileName(
                this,
                "Statistics log file",
                "",
                "Plain txt files (*.txt)",
                nullptr,
                QFileDialog::DontUseNativeDialog);

    ui->statisticsEdit->setText(filename);

    if (filename.isEmpty())
        ui->statisticsBox->setChecked(false);
}

void LogFileDialog::on_mergedBox_toggled(bool checked)
{
    if (!checked) {
        ui->mergedEdit->setText("");
        return;
    }

    QString filename = QFileDialog::getSaveFileName(
                this,
                "Merged ShelX file",
                "",
                "ShelX file (*.hkl)",
                nullptr,
                QFileDialog::DontUseNativeDialog);

    ui->mergedEdit->setText(filename);

    if (filename.isEmpty())
        ui->mergedBox->setChecked(false);
}

void LogFileDialog::on_unmergedBox_toggled(bool checked)
{
    if (!checked) {
        ui->unmergedEdit->setText("");
        return;
    }

    QString filename = QFileDialog::getSaveFileName(
                this,
                "Unmerged ShelX file",
                "",
                "ShelX file (*.hkl)",
                nullptr,
                QFileDialog::DontUseNativeDialog);

    ui->unmergedEdit->setText(filename);

    if (filename.isEmpty())
        ui->unmergedBox->setChecked(false);
}
