Kolmogorov-Smirnov Test
#######################

.. currentmodule:: datasketches

A `Kolmogorov-Smirnov Test <https://en.wikipedia.org/wiki/Kolmogorov%E2%80%93Smirnov_test>`_` is
a test of equality of two distributions to determine if they are likely to have come from the same
underlying distribution.
The DataSketches library provides a modified form of the test that takes into account the error
in each underlying sketch in the analysis.

Currently, the test assumes both input sketches are of the same family and data type.

.. autofunction:: ks_test
