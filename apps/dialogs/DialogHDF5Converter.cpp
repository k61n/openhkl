//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogHDF5Converter.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QString>
#include <QStringList>

#include "core/experiment/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentTypes.h"
#include "core/loader/DataReaderFactory.h"
#include "core/logger/Logger.h"
#include "core/resources/Resources.h"
#include "core/utils/Path.h"

#include "apps/dialogs/DialogHDF5Converter.h"
#include "apps/models/DataItem.h"
#include "ui_DialogHDF5Converter.h"

DialogHDF5Converter::DialogHDF5Converter(const nsx::DataList& numors, QWidget* parent)
    : QDialog(parent), _ui(new Ui::DialogHDF5Converter), _numors(numors)
{
    _ui->setupUi(this);

    _ui->outputDirectory->setText(QDir::currentPath());

    connect(_ui->browseOutputDirectory, SIGNAL(clicked()), this, SLOT(browseOutputDirectory()));

    connect(
        _ui->cancelOK, SIGNAL(clicked(QAbstractButton*)), this,
        SLOT(slotActionClicked(QAbstractButton*)));
}

DialogHDF5Converter::~DialogHDF5Converter()
{
    delete _ui;
}

void DialogHDF5Converter::slotActionClicked(QAbstractButton* button)
{
    auto button_role = _ui->cancelOK->standardButton(button);

    switch (button_role) {
    case QDialogButtonBox::StandardButton::Apply: {
        convert();
        break;
    }
    case QDialogButtonBox::StandardButton::Cancel: {
        reject();
        break;
    }
    case QDialogButtonBox::StandardButton::Ok: {
        accept();
        break;
    }
    default: {
        return;
    }
    }
}

void DialogHDF5Converter::convert()
{
    if (_numors.size() == 0) {
        QMessageBox::warning(this, "Output directory", "No numors selected for conversion");
        return;
    }

    QString outputDirectory = _ui->outputDirectory->text();
    if (outputDirectory.isEmpty()) {
        QMessageBox::warning(this, "Output directory", "Please enter an output directory.");
        return;
    }

    _ui->progressBar_conversion->setMaximum(_numors.size());
    _ui->progressBar_conversion->setValue(0);

    int comp(0);
    for (auto numor : _numors) {

        auto&& numor_filename = numor->filename();

        QFileInfo fileinfo(QString::fromStdString(numor_filename));
        QString basename = fileinfo.baseName();

        auto hdf5_filename = QDir(outputDirectory).filePath(basename + ".h5").toStdString();

        if (hdf5_filename.compare(numor_filename) == 0)
            return;

        try {
            numor->saveHDF5(hdf5_filename);
        } catch (...) {
            nsx::error() << "The filename " << hdf5_filename
                         << " could not be saved. Maybe a permission problem.";
        }
        _ui->progressBar_conversion->setValue(++comp);
    }
}

void DialogHDF5Converter::browseOutputDirectory()
{
    QString outputDirectory = QFileDialog::getExistingDirectory(
        this, "Enter output directory", ".", QFileDialog::ShowDirsOnly);
    if (!outputDirectory.isEmpty())
        _ui->outputDirectory->setText(outputDirectory);
}
