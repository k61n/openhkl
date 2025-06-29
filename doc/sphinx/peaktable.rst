.. _peaktable:

Peak Table
===========

The peak table features in many parts of the workflow, and contains (with a few
exceptions) the following columns.

.. list-table:: Peak table fields
   :widths: 10, 20
   :header-rows: 1
   :align: left

   * - Abbreviation
     - Description
   * - *h*
     - *h* Miller index
   * - *k*
     - *k* Miller index
   * - *l*
     - *l* Miller index
   * - *x*
     - x coordinate of unmerged peak (pixels)
   * - *y*
     - y coordinate of unmerged peak (pixels)
   * - *frame*
     - frame coordinate of unmerged peak
   * - I (sum)
     - Integrated (corrected) intensity from pixel sum integration
   * - I (profile)
     - Integrated (corrected) intensity from profile integration
   * - :math:`\sigma` (sum)
     - Variance from pixel sum integration
   * - :math:`\sigma` (profile)
     - Variance from profile integration
   * - :math:`I/\sigma` (sum)
     - Strength from pixel sum integration
   * - :math:`I/\sigma` (profile)
     - Strength from profile integration
   * - *B* (sum)
     - Integrated backround from pixel sum integration
   * - *B* (profile)
     - Integrated backround from profile integration
   * - :math:`\sigma` (B, sum)
     - Background variance from pixel sum integration
   * - :math:`\sigma` (B, profile)
     - Background variance from profile integration
   * - Gradient
     - Background gradient (image plane only, pixel sum integration only)
   * - :math:`\sigma` (Grad.)
     - Background gradient variance
   * - Data set
     - Data set associated with this peak
   * - Unit cell
     - Unit cell associated with this peak
   * - *d*
     - Resolution associated with this peak (*d* from Bragg's law)
   * - Reason for rejection
     - Reason, if any, why this peak is marked as invalid
   * - Caught by filter
     - Whether this peak was caught by a filter

Note that not all of these columns appear in every instance of the peak table,
but this is an exhaustive list of possible fields.

Rejection flags
===============

A peak flagged as "invalid" will not be integrated and is therefore not included
in the merge statistics. The data quality statistics (R-factors and CC-values)
can be indefinitely improved by simply regjected peaks that make them worse, so
some measure of how many peaks were rejected is necessary to ensure the
integrity of the data; this is the completeness, and it is the number of valid
peaks as a fraction of the total number of possible peaks. There are many
possibly reasons why a peak might be rejected; these are listed below.

Rejected during peak finding
----------------------------

* **Masked by user** - the peak intersects a mask applied to the detector image.
  Such masks are applied to exclude problematic regions of the detector, such as
  seams between detector plates and the area surrounding the beam stop.

* **Too many or few detector counts** - a blob must have a number of counts
  within a given range to qualify as a valid peaks. The upper limit is used to
  exlude pixels that have been saturated, i.e. the count is beyond the limit of
  the detector.

* **Peak centre outside frame range** - the centre of mass of the blob falls
  outside the angular region encompassed by the detector images.

* **Peak centre outside detector image** - the centre of mass of the blob falls
  outside the bounds of the detector image.

Rejected during integration
---------------------------

* **Integration failed** - This is a fall-through condition met if the
  integration fails but not for any other of the listed reasons. Should not
  generally happen.

* **Too few points to integrate** - the integration region contains too few
  counts to reliably integrate.

* **No unit cell assigned** - the given peak has no unit cell assigned and can
  not be handled.

* **No associated data set** - The given peak has no data set assigned and can
  not be handled.

* **Integration region extends beyond image/frame range** - The integration
  region extends either beyond the detector image, or outside the frame
  (rotation) range.

* **Negative, zero, or undefined background sigma** - certain rare circumstances
  might lead to a mathematically undefined sigma.

* **Peak contains saturated pixel** - A 16-bit detector can hold a maximum of
  65535 counts per pixel. Moreover, there is physical limit to the number of
  counts a detector pixel can reach. If the integrator is instructed to reject
  peaks containing such pixels, this reason is used.

* **Adjacent peak intensity region overlaps this peak** - if two peak intensity
  regions overlap, both peaks are rejected.

* **Centre of mass of peak is inconsistent** - the centre of
  mass of the peak is mathematically undefined.

* **Covariance matrix of peak is inconsistent** - the shape of the ellipsoid
  defining the peak, i.e. the covariance matrix, is mathematically undefined.

* **Shape of peak is too small or large** - the eigenvalues of the covariance
  matrix of the ellipsoid are too small or large.

* **Failed to find a minimum of I/Sigma** - specific to the :math:`\sigma/I`
  integrator, which requires a minimum in :math:`I/\sigma` to determine the
  extents of the integration region.

* **Peak centre moved beyond bounds of data set** - specific to the Gaussian
  integrator: the peak centre may move outside the bounds of the image or frame
  range during integration.

* **Pearson coefficient of fit is too low** - specific to the Gaussian
  Integrator: the fit of the peak shape is not good enough.

* **No shape model found** - No shape model was specified during profile
  integration.

Rejected during shape prediction
--------------------------------

* **No neighbouring profile to compute shape** - There are no neighbouring peaks
  within the specified pixel and frame range cutoffs with which to compute a
  mean covariance.

* **Too few neighbouring profiles to compute shape** There are too few
  neighbouring peaks within the specified pixel and frame range cutoffs with
  which to compute a mean covariance.

Rejected during refinement
--------------------------

* **Failure updating prediction post-refinement** - the reflection no longer
  intersects the Ewald sphere after refinement.

Other
-----

* **Frame coordinate interpolation failed** - The fractional frame value (i.e.
  specific rotation angle) cannot be determined because the ellipsoid extends
  beyond the frame range.

* **Manually unselected by user** - peak can be manually deselected form the
  detector image by selecting one or many peaks with the rectangle select tool,
  then pressing backspace.

* **Outside indexing tolerance** - The non-integer Miller indices (the product
  of a reciprocal vector and reciprocal basis) are too far from the specified
  integer Miller indices.
