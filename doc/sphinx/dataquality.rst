.. _dataquality:

Measures of Data Quality
========================

Refinement (R) values in crystallography measure the agreement between
observed predicted data :cite:`d-Evans2011`. The newer :math:`CC` correlation
coefficients estimate the correlation betwen an observed data set and the
``true'' signal providing a better indication of statistical validity,
especially in the high resolution limit :cite:`d-Karplus2012`.

:math:`R` factors
-----------------

Three different :math:`R` factors are computed in OpenHKL. The subscript
:math:`l` denotes observations of reflection :math:`h` with intensity :math:`I`.

1. :math:`R_\mathrm{merge}`:

   The traditional measure of internal consistency; this measure has the flaw of
   increasing with data multiplicity, although this can be mitigated by
   averaging more observations.

.. math::

   R_\mathrm{merge} = \frac{\sum_h\sum_l |I_{hl} - \langle I_h \rangle |}
                      {\sum_h \sum_l \langle I_h \rangle}

2. :math:`R_\mathrm{meas}`:

  This is the multiplicity-weighted :math:`R` factor, in which :math:`n_h` is the number
  of observations of reflection :math:`h`.

.. math::

   R_\mathrm{merge} = \frac{\sum_h\sum_l \left( \frac{n_h}{n_h - 1} \right)^{1/2}
                      |I_{hl} - \langle I_h \rangle |}
                      {\sum_h \sum_l \langle I_h \rangle}

3. :math:`R_\mathrm{pim}`:

   This is the precision-indicating :math:`R` factor, which estimates data
   quality after merging.

.. math::

   R_\mathrm{merge} = \frac{\sum_h\sum_l \left( \frac{1}{n_h - 1} \right)^{1/2}
                      |I_{hl} - \langle I_h \rangle |}
                      {\sum_h \sum_l \langle I_h \rangle}

The correlation coefficients :math:`CC_{1/2}` and :math:`CC\ast`
----------------------------------------------------------------

The statistic :math:`CC_{1/2}` as introduced in
 :cite:`d-Karplus2012`. The statistic :math:`CC_{1/2}` is defined as
follows. Randomly divide the unmerged dataset into two subsets. For each
symmetry-equivalence class :math:`[hkl]`, we have a merged intensity
:math:`x_{hkl}` from the first set and :math:`y_{hkl}` from the second
set. :math:`CC_{1/2}` is defined as the Pearson correlation coefficient
of the joint measurements :math:`(x_{hkl}, y_{hkl})`.

.. math:: CC_{1/2} := \rho(x, y) = \frac{\mathrm{Cov}(x, y)}{\sigma_x \sigma_y},

where :math:`\rho` denotes the Pearson correlation coefficient. Note
that this depends on the choice of division of the unmerged datasets
into two subsets, so it is itself a random variable. (However, under
some assumptions, one can check that its variance should be small.)

Let :math:`J_{hkl}` denote the true intensity (we use :math:`J` instead
of :math:`I` to distinguish this from our measured and/or merged
intensities). Then define random variables :math:`\xi := x - J` and
:math:`\eta := y - J`. We make the following assumption: :math:`\xi` and
:math:`\eta` are independent with mean zero, that :math:`\sigma_\xi =
\sigma_\eta`, and that :math:`\xi,\eta` are uncorrelated with :math:`J`.

Since :math:`\xi,\eta` are uncorrelated with :math:`J`,

.. math::

   \begin{aligned}
     \sigma^2_x &= \sigma^2_J + \sigma^2_\xi \\
     \sigma^2_y &= \sigma^2_J + \sigma^2_\eta = \sigma^2_J + \sigma^2_\xi\end{aligned}

Then

.. math::

   \begin{aligned}
     \rho(x,y)
     &= \frac{\mathrm{Cov}(x, y)}{\sigma_x \sigma_y} \\
     &= \frac{\mathrm{Cov}(J + \xi, J + \eta)}{\sigma_x \sigma_y} \\
     &= \frac{\sigma^2_J + \mathrm{Cov}(\xi, J) + \mathrm{Cov}(\eta, J) + \mathrm{Cov}(\xi, \eta)}{\sigma_x \sigma_y} \\
     &= \frac{\sigma^2_J}{\sigma^2_J + \sigma^2_\xi}\end{aligned}

Thus we have

.. math::

   \label{cc-half-simplified}
     CC_{1/2} = \sigma^2_J / \left(\sigma^2_J + \sigma^2_\xi \right)

This expression will be useful in the following subsection.

:math:`CC_\mathrm{true}`
------------------------

Let :math:`x, y, \xi, \eta, J` be as in the previous subsection. Define

.. math:: I = \frac{x+y}{2}

denote the merged intensities of the entire dataset. Then
:math:`CC_\mathrm{true}` is defined to be the Pearson correlation coefficient of
:math:`I` and the true intensities :math:`J`:

.. math::

   \label{cc-true-definition}
     CC_\mathrm{true} = \rho(I, J) = \frac{\mathrm{Cov}(I, J)}{\sigma_I \sigma_J}

Since in most cases we do not know the true intensities, this definition
is not directly useful.

Making the same assumptions about measurement error as in the previous
subsection, we have

.. math::

   \begin{aligned}
     \sigma^2_z &= \frac{1}{4} \sigma^2_x + \frac{1}{4}\sigma^2_y  + \frac{1}{2} \mathrm{Cov}(x, y) \\
     &= \sigma_J^2 + \frac{1}{2} \sigma_\xi^2\end{aligned}

and furthermore,

.. math:: \mathrm{Cov}(I, J) = \mathrm{Cov}(J + \frac{\xi+\eta}{2}, J) = \sigma^2_J.

Therefore,

.. math:: CC_\mathrm{true} = \frac{\sigma_J}{\sqrt{\sigma^2_J + \frac{1}{2}\sigma^2_\epsilon}}.

From equation  `[cc-half-simplified] <#cc-half-simplified>`__, we can
express :math:`\sigma^2_\xi` as :math:`\sigma^2_J(1/CC_{1/2}-1)`. Putting
this into the above expression for :math:`CC_\mathrm{true}`, we have

.. math::

   \begin{aligned}
     CC_\mathrm{true} &= \frac{\sigma_J}{\sqrt{\sigma_J^2 + \frac{1}{2}\sigma^2_\xi}}
     = \frac{\sigma_J}{\sqrt{\sigma_J^2 + \frac{1}{2}\sigma^2_J(1/CC_{1/2}-1)}} \\
     &= \frac{1}{\sqrt{\frac{1}{2}-\frac{1}{2 CC_{1/2}}}}
     = \sqrt{\frac{2 CC_{1/2}}{1+CC_{1/2}}},\end{aligned}

which amazingly is a function of :math:`CC_{1/2}` only. We therefore
define

.. math::

   \label{cc-star-definition}
     CC\ast := \sqrt{\frac{2 CC_{1/2}}{1+CC_{1/2}}},

to be an estimate of :math:`CC_\mathrm{true}`, which can be calculated directly
from the data. The statistic was introduced in
:cite:`d-Karplus2012`.


.. bibliography:: references.bib
    :cited:
    :labelprefix: D
    :keyprefix: d-
    :style: unsrt

Go to :ref:`top <dataquality>`.
