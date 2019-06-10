//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogAbsorption.h
//! @brief     Defines class DialogAbsorption
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <QDialog>
#include <QString>

#include "core/instrument/InstrumentTypes.h"

namespace nsx {
class ConvexHull;
}

namespace Ui {
class DialogAbsorption;
}

class CrystalScene;

class DialogAbsorption : public QDialog {
    Q_OBJECT

public:
    explicit DialogAbsorption(nsx::sptrExperiment experiment, QWidget* parent = nullptr);
    const std::string& getMovieFilename() const;
    ~DialogAbsorption();
signals:
    //! Emitted when the image is changed in the movie
    void loadImage(QString image);
    void angleText(QString text);
    void exportHull(const nsx::ConvexHull& hull);
public slots:
    void initializeSlider(int i);
private slots:
    void on_button_openFile_pressed();
    void setupInitialButtons();

private:
    Ui::DialogAbsorption* ui;
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
    CrystalScene* _cscene;
};
