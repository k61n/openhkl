.. _about:

About OpenHKL
=============

What is OpenHKL?
----------------

OpenHKL is a GUI programme for data reduction of single crystal diffraction
experiment data. It takes a series of detector images collected over a range of
sample rotation angle increments and constructs a real space 3D model of the
detector spots. This is used to determine an exhaustive list of reflections
for sample unit cell, along with their Miller indices, integrated intensities
and variances.

Why use OpenHKL?
----------------

There are many existing programmes that perform similar data reductions; many
have been used extensively in the past and produce excellent results. However,
OpenHKL is distinguised by a unique combination of important features.

Natively handles neutron diffraction
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The OpenHKL project was started with the aim of developing a data reduction
programme specifically for neutron diffraction, which is technically more
problematic than X-ray diffraction in terms of data reduction. However, it can
be used equally well for X-ray diffraction data.

Open source and fully documented
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

OpenHKL is fully open source, and none of its functionality is a ``black box``.
All algorithms are fully documented, such that users will no longer be left
guessing as to what exactly OpenHKL does during the data reductgion process.
Modification and collaboration are encouraged, with the aim of improving OpenHKL
as a community.

Easily extensible for different detector geometries
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is easy to extend OpenHKL to include different detector geometries; each
instrument is described a `.yaml` machine file which contains the metadata and
physical setup.

Written in C++, with a Modern QT GUI
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

OpenHKL is written in C++ for excellent speed, using object-oriented principles
to facilitate extensibility. It has a modern GUI, making it user friendly for
the end user.

Core functionality available via Python API
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All core functionality can be accessed via a Python scripting API, with the aim
of facilitating i) automation of the data reduction process, and ii) systematic
testing and optimisation of data reduction parameters.


History of OpenHKL
------------------

OpenHKL was started by Laurent Chapon and Eric Pellegrini at Institut
Laue-Langevin in Grenoble under the name NSXTool (Neutron Single Xtal). It later
became a collaboration between ILL and Forschungszentrum J\"ulich at the Heinz
Maier-Leibnitz Zentrum (MLZ) in Garching. It was renamed to OpenHKL in 2022 to
emphasise that it it is intended also for X-ray diffraction as well as Neutron
diffraction.
