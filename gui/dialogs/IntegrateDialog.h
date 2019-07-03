//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/IntegrateDialog.h
//! @brief     Defines class IntegrateDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_DIALOGS_INTEGRATEDIALOG_H
#define GUI_DIALOGS_INTEGRATEDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QCR/widgets/controls.h>

class IntegrateDialog : public QDialog {
 public:
    IntegrateDialog();

    double backgroundBegin() { return bkgBegin->value(); }
    double backgroundScale() { return bkgScale->value(); }
    double peakScale() { return shapeScale->value(); }
    double minimumD() { return minD->value(); }
    double maximumD() { return maxD->value(); }
    double radius() { return searchRadius->value(); }
    double numberOfFrames() { return numFrames->value(); }
    bool fitCenter() { return fitCen->isChecked(); }
    bool fitCov() { return fitCovariance->isChecked(); }
    QString integrator() { return method->currentText(); }
    void setIntegrators(QStringList integr);

 private:
    QFormLayout* layout;
    QcrComboBox* method;
    QcrCheckBox* fitCen;
    QcrCheckBox* fitCovariance;
    QcrDoubleSpinBox* shapeScale;
    QcrDoubleSpinBox* bkgBegin;
    QcrDoubleSpinBox* bkgScale;
    QcrDoubleSpinBox* minD;
    QcrDoubleSpinBox* maxD;
    QcrDoubleSpinBox* searchRadius;
    QcrDoubleSpinBox* numFrames;

};

#endif // GUI_DIALOGS_INTEGRATEDIALOG_H
