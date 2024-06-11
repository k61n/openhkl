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

#include "core/data/DataTypes.h"
#include "core/detector/DetectorEvent.h"
#include "gui/models/ColorMap.h"
#include "tables/crystal/UnitCell.h"

#include <QGraphicsEllipseItem>
#include <QImage>

#include <optional>

// For the plotting part, better to have RowMajor matrix to use QImage scanline
// function and optimize cache hit.
typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMatrix;

namespace ohkl {
class Convolver;
class InstrumentState;
}

class MaskItem;
struct DetectorSceneParams;
// Make it easier to remove direct beam
class DirectBeamGraphic : public QGraphicsEllipseItem { };
// Make it easier to remove contour text
class ContourLabelItem : public QGraphicsTextItem {
 public:
    ContourLabelItem(const QString& label);
};

//! Container for settings and pointers for visualising DataSets
class DataSetGraphics {

 public:
    DataSetGraphics(DetectorSceneParams* params);

    void setColorMap(const std::string& name)
    {
        _color_map = std::unique_ptr<ColorMap>(new ColorMap(name));
    };

    //! Associate a DataSet with this graphics generator
    void setData(ohkl::sptrDataSet data) { _data = data; };
    //! Assign a unit cell to compute Miller index positions
    void setUnitCell(ohkl::UnitCell* cell) { _cell = cell; };
    //! Positions of direct beam
    void setBeam(std::vector<ohkl::DetectorEvent>* events) { _beam = events; };
    //! Positions of direct beam before refinement
    void setOldBeam(std::vector<ohkl::DetectorEvent>* events) { _old_beam = events; };
    //! DataSet getter
    ohkl::sptrDataSet data() const { return _data; };
    //! Return the matrix used to construct the current image
    RowMatrix currentFrame() const { return _current_frame; };

    //! Get the base image on which peaks etc are superimposed
    std::optional<QImage> baseImage(std::size_t frame_idx, QRect full);
    //! Generate a filtered/thresholded image using the specified convolver
    Eigen::MatrixXd filteredImage(RowMatrix image);
    //! Generate a tooltip for the current scene position
    std::optional<QString> tooltip(int col, int row);
    //! Get graphics marking per-frame position of the direct beam
    QVector<DirectBeamGraphic*> beamGraphics(std::size_t frame_idx);
    //! Get detector mask graphics
    QVector<MaskItem*> maskGraphics();

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
    //! Overlay of resolution contours
    std::optional<QImage> resolutionContours(int n_contours, double d_min, double d_max);
    //! Get a vector of labels for resolution contours
    QVector<ContourLabelItem*> resolutionLabels();

 private:
    ohkl::sptrDataSet _data;
    ohkl::UnitCell* _cell;
    std::unique_ptr<ColorMap> _color_map;

    DetectorSceneParams* _params;
    RowMatrix _current_frame;

    std::vector<ohkl::DetectorEvent>* _beam;
    std::vector<ohkl::DetectorEvent>* _old_beam;

    //! Convolver for image filtering
    std::unique_ptr<ohkl::Convolver> _convolver;

    //! Colour of direct beam
    QColor _beam_color;
    //! Colour of unrefined direct beam;
    QColor _old_beam_color;
    //! Size of direct beam
    double _beam_size;

    //! Vector of resolution value labels for resolution contour overlay
    std::vector<double> _resolution_labels;
    //! Vector of label positions for resolution contour overlay
    std::vector<QPoint> _resolution_label_positions;
};

#endif // OHKL_GUI_GRAPHICS_DATASETGRAPHICS_H
