//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/models/ColorMap.h
//! @brief     Defines class ColorMap
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_MODELS_COLORMAP_H
#define NSX_GUI_MODELS_COLORMAP_H

#include <Eigen/Dense>
#include <QImage>
#include <vector>

//! A colormap for the DetectorScene
class ColorMap {
 public:
    ColorMap(const std::string& name);
    ColorMap();
    ColorMap(const double* rgb);
    ~ColorMap() = default;

    inline QRgb color(double v, double vmax)
    {
        int i = int(v / vmax * 255.0);
        i = std::max(0, i);
        i = std::min(255, i);
        return qRgb(_rgb[3 * i + 0], _rgb[3 * i + 1], _rgb[3 * i + 2]);
    }

    inline QRgb log_color(double v, double vmax)
    {
        double t = std::max(v / vmax * 255.0, 0.0);
        int i = std::min(255, int(t + 0.5));
        return qRgb(_log_rgb[3 * i + 0], _log_rgb[3 * i + 1], _log_rgb[3 * i + 2]);
    }

    QImage matToImage(
        const Eigen::ArrayXXd& source, const QRect rect, double colorMax, bool log = false);

    //! Returns a specific ColorMap
    //! @param name : the name of the ColorMap that is to be returned
    static ColorMap getColorMap(const std::string& name);
    //! Returns all names of the ColorMaps
    static std::vector<std::string> getColorMapNames();

 private:
    std::vector<double> _rgb;
    std::vector<double> _log_rgb;
};

#endif // NSX_GUI_MODELS_COLORMAP_H
