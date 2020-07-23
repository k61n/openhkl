//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/IntegrateDialog.cpp
//! @brief     Implements class IntegrateDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/IntegrateDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>

IntegrateDialog::IntegrateDialog() : QDialog{}
{
    layout = new QFormLayout(this);
    fitCen = new QcrCheckBox(
        "adhoc_centerfit", "fit center (Gaussian, pixel sum)", new QcrCell<bool>(false));
    fitCovariance = new QcrCheckBox(
        "adhoc_covfit", "fit covariance (Gaussian, pixel sum)", new QcrCell<bool>(false));
    shapeScale = new QcrDoubleSpinBox("adhoc_peakShapeScale", new QcrCell<double>(0.0), 10, 5);
    bkgBegin = new QcrDoubleSpinBox("adhoc_integrateBkgBegin", new QcrCell<double>(0.0), 10, 5);
    bkgScale = new QcrDoubleSpinBox("adhoc_integrateBkgScale", new QcrCell<double>(0.0), 10, 5);
    minD = new QcrDoubleSpinBox("adhoc_intergateMinD", new QcrCell<double>(0.0), 10, 5);
    maxD = new QcrDoubleSpinBox("adhoc_integrateMaxD", new QcrCell<double>(0.0), 10, 5);
    searchRadius = new QcrDoubleSpinBox("adhoc_integrateRadius", new QcrCell<double>(0.0), 10, 5);
    numFrames = new QcrDoubleSpinBox("adhoc_integrateNumFrames", new QcrCell<double>(0.0), 10, 5);
    QDialogButtonBox* buttons =
        new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, Qt::Horizontal);
    layout->addRow("", fitCen);
    layout->addRow("", fitCovariance);
    layout->addRow("Peak shape scale", shapeScale);
    layout->addRow("Background begin", bkgBegin);
    layout->addRow("Peak background scale", bkgScale);
    layout->addRow("minimum d", minD);
    layout->addRow("maximum d", maxD);
    layout->addRow("search radius", searchRadius);
    layout->addRow("number of frames", numFrames);
    layout->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &IntegrateDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &IntegrateDialog::reject);
}

void IntegrateDialog::setIntegrators(QStringList integr)
{
    method = new QcrComboBox("adhoc_methods", new QcrCell<int>(0), integr);
    layout->insertRow(0, "Method", method);
}
