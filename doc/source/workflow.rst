.. _workflow:

Workflow
========

The workflow in NSXTool approximately follows the order of the icons in
the sidebar: find peaks, filter, autoindex, predict, refine, merge. The
‘\ ``Home``\ tab enables experiment creation and loading/saving, while
the ``Experiment`` tab allows inspection and editing of various aspects
of the physical experiment.

The virutual “experiment” is the highest level object, and contains all
information from the physical experiment (the data sets), plus any
derived/reduced data, such as the unit cell, peaks, indices, and merged
data statistics. This object can be saved at any stage in the workflow
by returning to the ``Home`` tab.

Home
----

The home tab shows a list of loaded experiments, and allows creation of
new experiments, the loading of a saved experiment state and saving an
existing experiment state.

-  ``Create new experiment`` opens a dialogue prompting the user to name
   the experiment and select the instrument used. The parameters
   specific to that instrument are loaded from a YAML-formatted
   instrument file found with the source code.

-  ``Load from file`` loads a ``hdf5`` file containing a *saved*
   experiment state.

-  ``Save current`` saves the current experiment state as a ``hdf5``
   file.

-  ``Save all`` saves all experiments in ``hdf5`` format.

Experiment
----------

Find peaks
----------

The initial peak search is essentially a pure image processing step,
with no crystallographic input. The technique is roughly as follows

#. Apply an image filter to subtract local background

#. Apply a threshold to the resulting image

#. Find connected components (“blobs”) of the resulting thresholded
   image

#. Merge blobs that overlap, according to some cutoff

In the first step, we apply a filter which consists of a central
circular region with positive weight, and an outer annular region with
negative weight. The weights are chosen so that the convolution computes
the local average of the circular region subtracted by the average of
the annular region, effectively giving a local background subtraction.
The radii of the circle and annulus may be specified by the user.

To find connected components, we use a standard blob-search algorithm,
as described e.g. on the wikipedia page (do we have a better reference
for this?) In the last step, we compute inertia ellipsoids for each
blob, and merge those blobs whose ellipsoids overlap, after a
user-defined scaling factor has been applied. The merging process is
repeated until there are no longer any overlapping ellipsoids.

The collision detection problem for ellipsoids is sped up by storing
them in an octree. The ellipsoid overlap detection is implemented using
the criterion described in **TODO: find literature**.

.. table:: Peak search parameters

   +-------------------+----------------+-------------------------------+
   | **Parameters**    | Unit           | Description                   |
   +===================+================+===============================+
   | **Threshold**     | counts         | Pixels with a value below the |
   |                   |                | threshold are discarded       |
   +-------------------+----------------+-------------------------------+
   | **Merging scale** | :math:`\sigma` | Peak scale in sigmas, to      |
   |                   |                | detect collisions between     |
   |                   |                | blobs                         |
   +-------------------+----------------+-------------------------------+
   | **Minimum size**  | integer        | Blob is discarded if it       |
   |                   |                | contains fewer points than    |
   |                   |                | this                          |
   +-------------------+----------------+-------------------------------+
   | **Maximum size**  | integer        | Blob is discarded if it       |
   |                   |                | contains more points than     |
   |                   |                | this                          |
   +-------------------+----------------+-------------------------------+
   | **Maximum width** | frames         | Blob is discarded if it spans |
   |                   |                | more frames than this         |
   +-------------------+----------------+-------------------------------+
   | **Kernel**        |                | Convolution kernel for peak   |
   |                   |                | search                        |
   +-------------------+----------------+-------------------------------+
   | **Parameters**    |                | parameters                    |
   |                   |                | :math:`r_1, r_2, r_3` for the |
   |                   |                | pixel sum integrator (see     |
   |                   |                | section                       |
   |                   |                | `[sec:                        |
   |                   |                | pixelsum] <#sec:pixelsum>`__) |
   +-------------------+----------------+-------------------------------+
   | **Start frame**   | frame          | Initial frame in range for    |
   |                   |                | peak finding                  |
   +-------------------+----------------+-------------------------------+
   | **End frame**     | frame          | Final frame in range for peak |
   |                   |                | finding                       |
   +-------------------+----------------+-------------------------------+

At this stage in the workflow, there are no available profiles to
perform an accurate integration. The found peaks are integrated at this
stage using the somewhat na"̈ive pixel sum integrator (see section
`[sec:pixelsum] <#sec:pixelsum>`__ for details). The profiles are
gennerated and accurate integration performed in subsequent steps.

.. table:: Integration parameters

   +-----------------+----------------+---------------------------------+
   | **Parameters**  | Unit           | Description                     |
   +=================+================+=================================+
   | **Peak end**    | :math:`\sigma` | End of peak region in detector  |
   |                 |                | coordinates                     |
   +-----------------+----------------+---------------------------------+
   | **Lower limit** | :math:`\sigma` | Beginning of background region  |
   |                 |                | in detector coordinates         |
   +-----------------+----------------+---------------------------------+
   | **Upper limit** | :math:`\sigma` | End of background region in     |
   |                 |                | detector coordinates            |
   +-----------------+----------------+---------------------------------+

Filter peaks
------------

The filter peaks tab allows the user to remove peaks that meet certain
criteria froma collection and save this subset as a new collection. The
following controls cause the filter to catch that have:

State
   a specific (hidden) state flag set to “true”

   -  Selected — unselected peaks are generally unfit for integration
      for some reason

   -  Masked — a peak is masked if it has been manually highlighted on
      on the detector view

   -  Predicted — the peak has been predicted as opposed to found via
      the peak search algorithm

   -  Indexed — the peak has a unit cell assigned

Indexed peak
   been indexed (i.e. have a unit cell assigned)

Strength
   a strength (:math:`I/\sigma`) in the specified range

d range
   a d value (Å) in the specified range

Frame range
   a frame value (i.e. image number) in the specified range

Sparse dataset
Merged peak significance
Overlapping
Extinct from spacegroup
Complementary selection

Autoindexing
------------

The unit cell is determined in this tab using the 1D Fourier transform
method :cite:`w-Steller1997`, and peaks are assigned Miller
indices.

The algorithm works as follows. We are given some set of
:math:`\mathbf{q}` vectors which lie approximately on a lattice, yet to
be determined. To find candidate lattice directions, we take a random
sample of directions. For each direction, we perform the orthogonal
projection of each :math:`\mathbf{q}` vector to the infinite line
specified by the direction. We then take a finite number of bins along
this line (the way the binning is performed can be controlled by
user-defined parameters), and then take FFT of the resulting histogram.
The histogram will be strongly periodic when the direction corresponds
to a lattice direction, so we identify lattice vectors by taking the
strongest Fourier modes of the histograms.

The FFT method produces a finite set of potential lattice vectors. To
find a basis, we enumerate over triples of these basis vectors and rank
them according to

#. The percentage of peaks that can be indexed (with integer indices)

#. The volume of the resulting unit cell

This provides a ranked list of candidate unit cells, from which the user
may choose.

.. table:: Autoindexing parameters

   +----------------------+---------------+-------------------------+
   | **Parameters**       | Unit          | Description             |
   +======================+===============+=========================+
   | **Frames**           | frame number  | Choose a limited subset |
   |                      |               | of images from the data |
   |                      |               | set. Fourier transform  |
   |                      |               | autoindexing tends to   |
   |                      |               | work best on a subset   |
   |                      |               | of images, typically    |
   |                      |               | :math:`\simeq` 5        |
   |                      |               | degrees of oscillation  |
   |                      |               | at the start of the     |
   |                      |               | range, or a few         |
   |                      |               | (:math:`\simeq 10`)     |
   |                      |               | frames.                 |
   +----------------------+---------------+-------------------------+
   | **D range**          | Å             | Peaks with q vectors    |
   |                      |               | outside this range will |
   |                      |               | not be using in         |
   |                      |               | indexing                |
   +----------------------+---------------+-------------------------+
   | **Strength**         |               | Peaks with strength     |
   |                      |               | (:math:`I/\sigma`)      |
   |                      |               | outside this range will |
   |                      |               | not be used in indexing |
   +----------------------+---------------+-------------------------+
   | **Gruber Tol.**      |               |                         |
   +----------------------+---------------+-------------------------+
   | **Niggli Tol.**      |               |                         |
   +----------------------+---------------+-------------------------+
   | **Find Niggli cell** | T/F           | Whether to find the     |
   |                      |               | Niggli primitive cell   |
   +----------------------+---------------+-------------------------+
   | **Max Cell dim.**    | Å             | Maximum length of *any* |
   |                      |               | lattice vector          |
   +----------------------+---------------+-------------------------+
   | **Q Vertices**       | integer       | Number of reciprocal    |
   |                      |               | space directions to     |
   |                      |               | search for lattice      |
   |                      |               | vector                  |
   +----------------------+---------------+-------------------------+
   | **Subdivisions**     | integer       | Number of reciprocal    |
   |                      |               | space bins for Fourier  |
   |                      |               | transform               |
   +----------------------+---------------+-------------------------+
   | **Unit Cells**       | integer       | Maximum number of unit  |
   |                      |               | cells to find           |
   +----------------------+---------------+-------------------------+
   | **Min Volume**       | Å\ :math:`^3` | Minimum unit cell       |
   |                      |               | volume                  |
   +----------------------+---------------+-------------------------+
   | **Indexing Tol.**    |               |                         |
   +----------------------+---------------+-------------------------+
   | **Frequency Tol.**   | 0.0 - 1.0     | Minimum fraction of     |
   |                      |               | amplitude of the zeroth |
   |                      |               | Fourier frequency to    |
   |                      |               | accept as a candidate   |
   |                      |               | lattice vector          |
   +----------------------+---------------+-------------------------+

The FFT indexing method can be difficult to use correctly because there
is no systematic method for reaching the correct solution, and there are
many adjustable parameters. As a guide, the follwing tend to have a
substantial effect on the success (or otherwise) of the procedure:

#. Number of peaks/number of frames: using too many peaks/frames tends
   to result in failure. This is obviously strongly dependent on the
   nature of the sample. For example, using the BioDiff detector, up to
   10 frames, containing no more than 300 peaks seems to be sufficient
   to index complicated biological crystals.

#. Subdivisions: The process is strongly dependent on the number of FFT
   histogram bins.

#. Q Vertices: This is the parameter that is most easy to systematically
   vary, since more Q vectors will increase the likelihood of finding
   one that is parallel to the normal to a lattice plane. Increasing
   this value will usually (but not invariably) enhance the odds of
   finding a lattice vector.

#. Frequency Tol: the FFT algorithm will discard any candidate
   reciprocal lattice vector whose amplitude is less than this fraction
   of the zeroth Fourier frequency. Use with care!

Predict peaks
-------------

Shape collection
~~~~~~~~~~~~~~~~

The shape collection is the set of “profiles” alluded to in section
`[sec:3dprofile] <#sec:3dprofile>`__ — i.e. strong peaks with a
well-defined shapes that are used to fit the weak peaks. In practice
this is simply a collection of integrated strong peaks. During profile
fitting integration of a weak peak, the integrator will compute a mean
of all profiles in the shape collection within a given radius of the
weak peak coordinates to use as the fitting profile in that instance.

The ``Build shape collection`` button simply filters out weak peaks and
integrates the shape collection. The ``Calculate profile button`` uses
the peak collection to compute the mean profile at the given coordinates
and neighbours within the given radius, plotting it in the box in the
bottom right. The slider scrolls through the frames in which the profile
is visible.

.. table:: Shape collection dialogue parameters

   +------------------------+----------------+-------------------------+
   | **Parameters**         | Unit           | Description             |
   +========================+================+=========================+
   | **Number along x/y/z** | integer        | Number of histogram     |
   |                        |                | bins for profile in     |
   |                        |                | x/y/z direction         |
   +------------------------+----------------+-------------------------+
   | **Kabsch coordinates** | T/F            | Toggle Kabsch           |
   |                        |                | coordinate system as    |
   |                        |                | opposed to detector     |
   |                        |                | coordinate system       |
   |                        |                | (applies only to        |
   |                        |                | Profile 3D and Profile  |
   |                        |                | 1D integrators)         |
   +------------------------+----------------+-------------------------+
   | **:math:`\sigma_D`**   |                | Peak variance due to    |
   |                        |                | beam divergence         |
   +------------------------+----------------+-------------------------+
   | **:math:`\sigma_M`**   |                | Peak variance due to    |
   |                        |                | crystal mosaicity       |
   +------------------------+----------------+-------------------------+
   | **Minimum strength**   |                | Exclude weak peaks with |
   |                        |                | strength                |
   |                        |                | (I/:math:`\sigma`)      |
   |                        |                | below this value        |
   +------------------------+----------------+-------------------------+
   | **Minimum d**          | Å              | Only include peaks      |
   |                        |                | above this d value      |
   +------------------------+----------------+-------------------------+
   | **Maximum d**          | Å              | Only include peaks      |
   |                        |                | below this d value      |
   +------------------------+----------------+-------------------------+
   | **Peak scale**         | :math:`\sigma` | Size of peak region     |
   +------------------------+----------------+-------------------------+
   | **Background begin**   | :math:`\sigma` | Size of beginning of    |
   |                        |                | background region       |
   +------------------------+----------------+-------------------------+
   | **Background end**     | :math:`\sigma` | Size of end of          |
   |                        |                | background region       |
   +------------------------+----------------+-------------------------+
   |                        |                |                         |
   +------------------------+----------------+-------------------------+
   | **x/y**                | pixels         | Compute mean profile    |
   |                        |                | for these detector x/y  |
   |                        |                | coordinates             |
   +------------------------+----------------+-------------------------+
   | **Frame**              | frame          | Compute mean profile    |
   |                        |                | for this frame          |
   |                        |                | coordinate coordinates  |
   |                        |                | (with x/y)              |
   +------------------------+----------------+-------------------------+
   | **Radius**             | pixels         | Detector image radius   |
   |                        |                | for neighbour search    |
   |                        |                | for computing mean      |
   |                        |                | profile                 |
   +------------------------+----------------+-------------------------+
   | **N frames**           | frame          | Detector image radius   |
   |                        |                | in frames for neighbour |
   |                        |                | search for computing    |
   |                        |                | mean profile            |
   +------------------------+----------------+-------------------------+

.. _predict-peaks-1:

Predict peaks
~~~~~~~~~~~~~

Given the unit cell, an exhaustive set of Miller indexed reflections can
be generated within the specified d range. Given the space group,
symmetry-forbidden reflections can be removed from this collection.

.. table:: Peak prediction parameters

   +------------------+--------+----------------------------------------+
   | **Parameters**   | Unit   | Description                            |
   +==================+========+========================================+
   | **Unit cell**    |        | Unit cell to predict peaks from        |
   +------------------+--------+----------------------------------------+
   | **Interpolaton** |        |                                        |
   +------------------+--------+----------------------------------------+
   | **d min**        | Å      | Only include peaks above this d value  |
   +------------------+--------+----------------------------------------+
   | **d max**        | Å      | Only include peaks below this d value  |
   +------------------+--------+----------------------------------------+
   | **Radius**       | pixels | Detector image radius for neighbour    |
   |                  |        | search for computing mean profile      |
   +------------------+--------+----------------------------------------+
   | **Frames**       | frame  | Detector image radius in frames for    |
   |                  |        | neighbour search for computing mean    |
   |                  |        | profile                                |
   +------------------+--------+----------------------------------------+

Integrate peaks
~~~~~~~~~~~~~~~

The peaks are integrated, ideally using a profile-fitting method
(althought the pixel sum integrator is available) to compute intensities
and sigmas.

| l l X **Parameters** & Unit & Description
| **Fit the center** & T/F &
| **Fit the covariance** & T/F &
| **Peak end** & :math:`\sigma` & End of peak region in detector
  coordinates
| **Bkg begin** & :math:`\sigma` & Beginning of background region in
  detector coordinates
| **Bkg end** & :math:`\sigma` & End of background region in detector
  **Minimum d** & Å & Only include peaks above this d value
| **Maximum d** & Å & Only include peaks below this d value
| **Search radius** & pixels & Detector image radius for neighbour
  search for computing mean profile
| **N. of frames** & frame & Detector image radius in frames for
  neighbour search for computing mean profile

Refine
------

Input
~~~~~

Parameters to refine
~~~~~~~~~~~~~~~~~~~~

Plot
~~~~

Update predictions
~~~~~~~~~~~~~~~~~~

Reintegrate peaks
~~~~~~~~~~~~~~~~~

Merge peaks
-----------


.. bibliography:: references.bib
    :cited:
    :labelprefix: W
    :keyprefix: w-
    :style: unsrt

Go to :ref:`top <workflow>`.
