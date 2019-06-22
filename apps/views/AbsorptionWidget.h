//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/views/AbsorptionWidget.h
//! @brief     Defines class AbsorptionWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <string>
#include <utility>
#include <vector>

#include <QWidget>

#include "core/instrument/InstrumentTypes.h"

namespace Ui {

class AbsorptionWidget;
}

class AbsorptionWidget : public QWidget {
    Q_OBJECT

 public:
    explicit AbsorptionWidget(nsx::Experiment* experiment, QWidget* parent = nullptr);
    ~AbsorptionWidget();

 private:
    Ui::AbsorptionWidget* ui;
    //! Link to the experiment
    nsx::Experiment* _experiment;
    //!
    nsx::RotAxis* _spindleAxis;
    //! Map of angle and
    std::vector<std::pair<double, std::string>> _imageList;
    //! Path of the file
    std::string _filepath;
    //! read info file containing video information
    void readInfoFile(const std::string& filename);
 public slots:
    void loadImage(unsigned int i);
    void initializeSlider(int i);
 private slots:
    void on_pushButton_clicked();
};
