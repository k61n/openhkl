#ifndef NSXTOOL_IBLOB_H_
#define NSXTOOL_IBLOB_H_

#include <iostream>

namespace nsx {

/* !
 * \brief Class IBlob.
 * Blobs are used to store region of interest in a 2D image.
 * A Blob is constructed by adding points in the image with coordinates x,y
 * and an associated mass that represents any scalar field such as intensity.
 * Blob objects in NSXTool records the total mass, the mass-weighted first and second moments
 * as new points are added to the blob. Knowledge about individual points is lost,
 * i.e Blob can only increase in size.
 * Blobs can be merged and maintain zero, first and second momentum.
 *
 */
class IBlob {
public:
  //!Initialize an empty blob
  IBlob();

  //! Copy constructor
  IBlob(const IBlob&);
  //! Move
  IBlob(IBlob&&) = default;
  //! Assignment
  IBlob& operator=(const IBlob&);
  //! Add point to the Blob
  void addPoint(double x, double y, double m);
  //! Merge a second blob
  void merge(const IBlob&);
  //! Return the total mass
  double getMass() const;
  //! Return the number of points
  int getComponents() const;
  //! Return the minimumMass
  double getMinimumMass() const;
  //! Return the minimumMass
  double getMaximumMass() const;

  void printSelf(std::ostream& os) const;

};

std::ostream& operator<<(std::ostream& os, const IBlob& b);

} // end namespace nsx

#endif // NSXTOOL_IBLOB_H_
