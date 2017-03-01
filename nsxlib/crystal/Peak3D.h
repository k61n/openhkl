/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon Institut Laue-Langevin
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
    j.fisher[at]fz-juelide.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef NSXTOOL_SIMPLEPEAK_H_
#define NSXTOOL_SIMPLEPEAK_H_

#include <map>
#include <memory>

#include <Eigen/Dense>

#include "UnitCell.h"
#include "../geometry/IShape.h"
#include "../geometry/Ellipsoid.h"
#include "../utils/Types.h"
#include "../geometry/IntegrationRegion.h"

namespace SX {

namespace Geometry {
class Blob3D;
}


namespace Data {
    class DataSet;
}

namespace Instrument {
    class Diffractometer;
    class ComponentState;
    class DetectorEvent;
    class Sample;
    class Detector;
    class Source;
}

namespace Crystal {

class PeakIntegrator;

class Peak3D {
public:
    using sptrShape3D=std::shared_ptr<SX::Geometry::IShape<double,3>>;
    using Ellipsoid3D=Geometry::Ellipsoid<double,3>;
    using sptrEllipsoid3D=std::shared_ptr<Ellipsoid3D>;
    using shape_type = SX::Geometry::IShape<double,3>;
    using IntegrationRegion = SX::Geometry::IntegrationRegion;


    Peak3D(std::shared_ptr<SX::Data::DataSet> data = nullptr);
    //Peak3D(std::shared_ptr<SX::Data::IData> data, const SX::Geometry::Blob3D& blob, double confidence);
    Peak3D(std::shared_ptr<SX::Data::DataSet> data, const Ellipsoid3D& shape);

    //! Copy constructor
    Peak3D(const Peak3D& other);

    //! Assignment operator
    Peak3D& operator=(const Peak3D& other);

    ~Peak3D() = default;
    //! Attach the data

    void linkData(const std::shared_ptr<SX::Data::DataSet>& data);

    //! Detach the data
    void unlinkData();

    //! Set the Peak region. Peak shaped is owned after setting
    void setShape(const Ellipsoid3D& peak);

    //! Get the Miller indices of the peak (double to allow integration of incommensurate peaks)
    Eigen::RowVector3d getMillerIndices() const;

    bool getMillerIndices(Eigen::RowVector3d& hkl, bool applyUCTolerance=true) const;

    bool getMillerIndices(int ucIndex, Eigen::RowVector3d& hkl, bool applyUCTolerance=true) const;

    bool getMillerIndices(const UnitCell& uc, Eigen::RowVector3d& hkl, bool applyUCTolerance=true) const;

    //! Get the integral Miller indices
    Eigen::RowVector3i getIntegerMillerIndices() const;

    //! Get kf vector in the frame of reference of the diffractometer
    Eigen::RowVector3d getKf() const;

    //! Get q vector in the frame of reference of the diffractometer
    Eigen::RowVector3d getQ() const;

    void getGammaNu(double& gamma,double& nu) const;

    //! Run the integration of the peak; iterate over the data
    //void integrate();

    std::shared_ptr<SX::Data::DataSet> getData() const { return _data.lock();}

    //! Get the projection of total data in the bounding box.
    Eigen::VectorXd getProjection() const;
    Eigen::VectorXd getPeakProjection() const;
    Eigen::VectorXd getBkgProjection() const;
    Eigen::VectorXd getProjectionSigma() const;
    Eigen::VectorXd getPeakProjectionSigma() const;
    Eigen::VectorXd getBkgProjectionSigma() const;

    const Ellipsoid3D& getShape() const { return _shape; }
    const SX::Geometry::IntegrationRegion& getIntegrationRegion() const { return _integrationRegion; }

    //! Return the scaled intensity of the peak.
    double getScaledIntensity() const;
    //! Return the raw intensity of the peak.
    double getRawIntensity() const;
    //! Returns the error on the raw intensity.
    double getRawSigma() const;
    //! Returns the error on the scaled intensity.
    double getScaledSigma() const;
    //!
    double getIOverSigmaI() const;
    //! Return the lorentz factor of the peak.
    double getLorentzFactor() const;
    std::shared_ptr<SX::Instrument::ComponentState> getSampleState();
    double getSampleStepSize() const;

    //! Return the scaling factor.
    double getScale() const;
    //! Rescale the current scaling factor by scale.
    void rescale(double factor);
    //! Set the scaling factor.
    void setScale(double factor);
    //!
    void setSampleState(const std::shared_ptr<SX::Instrument::ComponentState>& sstate);
    //!

    void setDetectorEvent(const SX::Instrument::DetectorEvent& event);
    //!
    void setSource(const std::shared_ptr<SX::Instrument::Source>& source);

    friend bool operator<(const Peak3D& p1, const Peak3D& p2);
    void setSelected(bool);
    bool isSelected() const;
    void setMasked(bool masked);
    bool isMasked() const;
    void setTransmission(double transmission);
    double getTransmission() const;

    void addUnitCell(std::shared_ptr<SX::Crystal::UnitCell> uc, bool activate=true);
    int getActiveUnitCellIndex() const;
    sptrUnitCell getActiveUnitCell() const;
    sptrUnitCell getUnitCell(int index) const;

    friend bool operator<(const Peak3D& p1, const Peak3D& p2);

    bool isIndexed() const;

    void setObserved(bool observed);
    bool isObserved() const;

    bool hasUnitCells() const;
    void scaleShape(double scale);

    // testing: new implementation of integration
    //void framewiseIntegrateBegin();
//    void framewiseIntegrateStep(Eigen::MatrixXi& frame, unsigned int idx);
//    void framewiseIntegrateEnd();

    //! update the integration
    void updateIntegration(const PeakIntegrator& integrator);

    //! compute P value that there is actually an observed peak, assuming Poisson statistics
    double pValue();

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
    //! Pointer to the data containing the peak
    std::weak_ptr<SX::Data::DataSet> _data;
    //! Miller indices of the peak
    // Eigen::RowVector3d _hkl;
    //! Shape describing the Peak zone
    Ellipsoid3D _shape;
    //! Region used to integrate the peak
    SX::Geometry::IntegrationRegion _integrationRegion;
    //! Shape describing the background zone (must fully contain peak)
    // Ellipsoid3D _bkg;

    //!
    Eigen::VectorXd _projection;
    Eigen::VectorXd _projectionPeak;
    Eigen::VectorXd _projectionBkg;

    //! Miller indices of the peak
//	Eigen::RowVector3d _hkl;
    //! Shape describing the Peak zone

    Eigen::VectorXd _pointsPeak;
    Eigen::VectorXd _pointsBkg;
    Eigen::VectorXd _countsPeak;
    Eigen::VectorXd _countsBkg;

    //!
    CellList _unitCells;
    //! Pointer to the state of the Sample Component

    std::shared_ptr<SX::Instrument::ComponentState> _sampleState;

    //! Detector Event state
    std::unique_ptr<SX::Instrument::DetectorEvent> _event;
    //!
    std::shared_ptr<SX::Instrument::Source> _source;

    double _counts;
    double _countsSigma;
    double _scale;
    bool _selected;
    bool _masked;
   // bool _calculated;
    bool _observed;
    double _transmission;
    int _activeUnitCellIndex;
};

using sptrPeak3D = std::shared_ptr<Peak3D>;

} // namespace Crystal
} // namespace SX

#endif /* NSXTOOL_SIMPLEPEAK_H_ */
