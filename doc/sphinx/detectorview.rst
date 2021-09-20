.. _detectorview:

Detector image widget
=====================

The ``Experiment``, ``Find peaks``, ``Peak filter``, ``Predict``, ``Refine`` and
``Integrate`` tabs all have detector view widgets that give *contextual
information* related to their stage in the workflow. The basic widget has three
control elements:

1. A combo box to select the data set
2. A scroll bar with which to scroll through the imagees in the data set
3. A spin box to select an image in the data set
4. A slider to set the maximum intensity of the image (i.e. change th contrast)

In any detector widget without the ``interaction mode`` combo box, dragging out
a rectangle will zoom in on a region with a fixed aspect ratio. The previous
zoom level can be returned by right-clicking.

Some detector views have additional widgets:

Experiment
~~~~~~~~~~

The ``Experiment`` widget has an additional combo box to select the
``Interaction mode``. This presents several options:

1. ``Zoom`` --- drawing a rectangle on the detector image will zoom in to the
   selected region. Note that the aspect ratio is constrained, so the zoomed
   image segment will not correspond exactly to the selection.

2. ``Selection`` --- drawing a rectangle will select any objects superimposed onto
   the detector image, namely peaks and masks. Hitting ``Backspace`` will
   *deselect* any peaks with centres inside the rectangle, and *delete* any
   masks that collide with the rectangle. Clicking anywhere will dismiss the
   selection box.

3. ``Rectangular mask`` --- draw a rectangular mask on the detector image. When
   runninng ``Find peaks``, any peaks in these regions will be flagged as
   ``masked`` and ignored in any subsequent processing.

4. ``Elliptical mask`` --- draw a elliptical mask on the detector image with the
   given rectangular bounding box. When runninng ``Find peaks``, any peaks in
   these regions will be flagged as ``masked`` and ignored in any subsequent
   processing.

5. ``Line plot`` --- draw a line along the plot; the intensity along the line is
   plotted in the graph widget below.

6. ``Horizontal slice`` --- draw a rectangle on the detector image; the columns are
   integrated, resulting in an intensity plot along the horizontal axis.

7. ``Vertical slice`` --- draw a rectangle on the detector image; the rows are
   integrated, resulting in an intensity plot along the vertical axis.


Find Peaks
~~~~~~~~~~

The ``Find peaks`` widget has an additional combo box to select the
``Interaction mode``. This presents several options:

1. ``Zoom`` --- drawing a rectangle on the detector image will zoom in to the
   selected region. Note that the aspect ratio is constrained, so the zoomed
   image segment will not correspond exactly to the selection.

2. ``Selection`` --- drawing a rectangle will select any objects superimposed onto
   the detector image, namely peaks and masks. Hitting ``Backspace`` will
   *deselect* any peaks with centres inside the rectangle, and *delete* any
   masks that collide with the rectangle. Clicking anywhere will dismiss the
   selection box.

3. ``Rectangular mask`` --- draw a rectangular mask on the detector image. When
   runninng ``Find peaks``, any peaks in these regions will be flagged as
   ``masked`` and ignored in any subsequent processing.

4. ``Elliptical mask`` --- draw a elliptical mask on the detector image with the
   given rectangular bounding box. When runninng ``Find peaks``, any peaks in
   these regions will be flagged as ``masked`` and ignored in any subsequent
   processing.

Detector window
~~~~~~~~~~~~~~~

The non-contextual ``Detector window`` (available from the ``View`` menu), has
an additional combo to control the ``Cursor mode``, i.e. the information
displayed as a tooltip when hovering over a pixel in the detector image. The
options are,

1. ``Pixel`` --- the pixel coordinates (x, y) followed by the count of the
   pixel

2. :math:`\theta` --- the diffraction angle :math:`\theta` followed by the
   count

3. :math:`\gamma / \nu` --- the instrument angles :math:`\gamma` and
   :math:`\nu`, followed by the count

4. ``d`` --- The lattice spacing ``d`` from Bragg's law followed by the
   count

5. ``Miller index`` --- the non-integer Miller index :math:`(h k l)` followed by
   the count

Show/hide peaks widget
======================

Every detector widget has an assocated widget to change the peak visualisatioon
settings on the control panel on the left of the application, in most cases,
this is labelled ``Show/hide peaks``. It enables control of three different
aspects of the information superimposed on the detector image:

Valid peaks
~~~~~~~~~~~

Plot peak centres as circles on the detector image.

1. Show or hide the valid peaks (peaks which are ``selected`` and *not*
   ``masked``)
2. Change the size of the circle indicating the centre of the peak
3. Change the colour of the circle indicating the centre of the peak

Integration Regions
~~~~~~~~~~~~~~~~~~~

Generate an overlay indicating the peak intensity regions (default green) and
background regions (default yellow).

1. Show or hide the integration regions
2. Change the alpha (transparency) of the integration regions; 0 = transparent,
   1 = opaque.
3. Change the colour of the integration regions, respectively the peak
   (intensity) region and the background region.
4. Preview the integration regions. If this is checked, the integration
   parameters below are used to determine the size of the integration regions.
   If unchecked, the integration regions are determined from the either the
   default parameters, or the paremeters used last time the peaks were
   integrated.
5. Integration region size parameters: these control the size of the
   peak/intensity region, the start of the background region and the end of the
   background region in sigmas. See :ref:`sec_peakshape`.

Invalid peaks
~~~~~~~~~~~~~

Plot peaks that have for some reason been disabled as circles on the detector
image.

1. Show or hide the invalid peaks (peaks which are ``masked`` or *not* selected.
2. Change the size of the circle indicating the centre of the peak
3. Change the colour of the circle indicating the centre of the peak
