//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/DataSetGraphics.h
//! @brief     Defines class DataSetGraphics
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_DATASETGRAPHICS_H
#define OHKL_GUI_GRAPHICS_DATASETGRAPHICS_H

#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/instrument/InstrumentState.h"
#include "gui/graphics/DetectorSceneParams.h"
#include "gui/models/ColorMap.h"
#include "tables/crystal/UnitCell.h"

#include <QImage>

#include <optional>

typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMatrix;

//! Container for settings and pointers for visualising DataSets
class DataSetGraphics {

 public:
    DataSetGraphics(DetectorSceneParams* params);

    void setColorMap(const std::string& name) {
        _color_map = std::unique_ptr<ColorMap>(new ColorMap(name));
    };

    void setData(ohkl::sptrDataSet data) { _data = data; };
    void setUnitCell(ohkl::UnitCell* cell) { _cell = cell; };
    ohkl::sptrDataSet data() const { return _data; };
    RowMatrix currentFrame() const { return _current_frame; };

    //! Get the base image on which peaks etc are superimposed
    std::optional<QImage> baseImage(std::size_t frame_idx, QRect full);
    //! Generate a tooltip for the current scene position
    std::optional<QString> tooltip(int col, int row);


    //! Get the count of a specific pixel
    int pCount(int col, int row);
    //! Get the instrument state for a specific frame
    std::optional<ohkl::InstrumentState> instrumentState();
    //! Intensity of a specific pixel
    std::optional<QString> intensity(int col, int row);
    //! Gamma and nu angles of a specific pixel
    std::optional<QString> gammaNu(int col, int row);
    //! Theta angle of a specific pixel
    std::optional<QString> twoTheta(int col, int row);
    //! D value of a specific pixel
    std::optional<QString> dSpacing(int col, int row);
    //! Miller indices of a specific pixel
    std::optional<QString> millerIndices(int col, int row);

 private:
    ohkl::sptrDataSet _data;
    ohkl::UnitCell* _cell;
    std::unique_ptr<ColorMap> _color_map;

    DetectorSceneParams* _params;
    RowMatrix _current_frame;
};

#endif // OHKL_GUI_GRAPHICS_DATASETGRAPHICS_H
