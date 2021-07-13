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

#ifndef NSX_GUI_DIALOGS_ABSORPTIONDIALOG_H
#define NSX_GUI_DIALOGS_ABSORPTIONDIALOG_H

#include "gui/graphics/CrystalScene.h"
#include <QDialog>
#include <QScrollBar>

namespace nsx {
class Experiment;
class RotAxis;
}

class AbsorptionDialog : public QDialog {
    Q_OBJECT
 public:
    AbsorptionDialog();

    void initializeSlider(int i);

 private:
    void on_button_openFile_pressed();
    void setupInitialButtons();
    //! Link to the experiment
    nsx::Experiment* _experiment;
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
    QPushButton* rulerButton;
    QPushButton* pickCenterButton;
    QPushButton* pickPointButton;
    QPushButton* removePointButton;
    QPushButton* triangulateButton;
};

#endif // NSX_GUI_DIALOGS_ABSORPTIONDIALOG_H
