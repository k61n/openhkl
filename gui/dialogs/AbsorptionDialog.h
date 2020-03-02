//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/AbsorptionDialog.h
//! @brief     Declares class AbsorptionDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_DIALOGS_ABSORPTIONDIALOG_H
#define GUI_DIALOGS_ABSORPTIONDIALOG_H

#include "core/instrument/InstrumentTypes.h"
#include "gui/graphics/CrystalScene.h"
#include <QCR/widgets/actions.h>
#include <QDialog>
#include <QScrollBar>

class AbsorptionDialog : public QDialog {
    Q_OBJECT
 public:
    AbsorptionDialog();

    void initializeSlider(int i);

 private:
    void on_button_openFile_pressed();
    void setupInitialButtons();
    //! Link to the experiment
    nsx::sptrExperiment _experiment;
    //! Rotation axis to collect movie
    nsx::RotAxis* _spindleAxis;
    //! Set of Roatation angle and absolute fileName for jpg image
    std::vector<std::pair<double, std::string>> _imageList;
    //! Path of the file
    std::string _filepath;
    //! read info file containing video information
    void readInfoFile(const std::string& filename);
    //! Pointer to the QGraphicsScene
    CrystalScene* crystalScene;

    QScrollBar* scrollBar;
    QcrIconTriggerButton* rulerButton;
    QcrIconTriggerButton* pickCenterButton;
    QcrIconTriggerButton* pickPointButton;
    QcrIconTriggerButton* removePointButton;
    QcrIconTriggerButton* triangulateButton;
};

#endif // GUI_DIALOGS_ABSORPTIONDIALOG_H
