//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/raw/XFileHandler.h
//! @brief     Defines class XFileHandler
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_LOADER_XFILEHANDLER_H
#define NSX_CORE_LOADER_XFILEHANDLER_H

#include <Eigen/Dense>
#include <memory>
#include <vector>

namespace ohkl {

std::vector<std::string> tokenize(const std::string& s);

class XFileHandler {
 public:
    XFileHandler(std::string filename);

    //! Set the filename
    void setName(std::string filename) { _filename = filename; };
    //! Parse the .x file
    void readXFile(int frame);
    //! Return a list of peak centres for plotting
    std::vector<Eigen::Vector3d> getPeakCenters();
    //! Return the frame number
    int frame() { return _frame; };
    /* void writeXFile(); */

    struct Reflection {
        const static std::string stop_flag;
        //! Miller index h
        int h;
        //! Miller index k
        int k;
        //! Miller index l
        int l;
        //! true = partial reflection, false = full reflection
        bool partial;
        //! intensity from profile integration
        double I_profile;
        //! Intensity from summation
        double I_summation;
        //! Chi-squared loss from profile fitting
        double chisq;
        //! Sigma of I
        double sigma;
        //! cos(angle at detector)
        double cos_detector_angle;
        //! x-coordinate of predicted centroid (pixels)
        double px;
        //! y-coordinate of predicted centroid (pixels)
        double py;
        //! Lorentz polarisation obliquity factor
        double obliquity;
        //! Strength of averaged profile
        double strength;
        //! Mask size in pixels (x direction)
        int maskx;
        //! Mask size in pixels (y direction)
        int masky;

        bool parse(std::string line);
    };

 private:
    //! .x file name
    std::string _filename;
    //! Frame number of .x file
    int _frame;
    //! Material name
    std::string _name;
    //! a matrix orientation
    Eigen::Matrix3d _a;
    //! u matrix orientation
    Eigen::Matrix3d _u;
    //! Oscillation start
    double _osc_start;
    //! Oscillation end
    double _osc_end;
    //! Distance in pixels
    double _dist_pixels;
    //! Wavelength
    double _wavelength;
    //! Crystal rotation x
    double _rotx;
    //! Crystal rotation y
    double _roty;
    //! Crystal rotation z
    double _rotz;
    //! Mosaicity
    double _mosaicity;
    //! Vector of reflections
    std::vector<Reflection> _reflections;
    //! Integration mask
    Eigen::MatrixXi _mask;
};

// TODO: generalise to other data fomrats?
class PeakCenterDataSet {
 public:
    PeakCenterDataSet() = default;

    void init(int nframes);
    XFileHandler* getFrame(int frame);
    void addFrame(std::string filename, int frame);

 private:
    std::vector<std::unique_ptr<XFileHandler>> _xfiles;
};

} // namespace ohkl

#endif // NSX_CORE_LOADER_XFILEHANDLER_H
