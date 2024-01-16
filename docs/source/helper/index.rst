Helper Classes
==============

.. currentmodule:: dataksetches

These classes are required for certain sketches or specific
functionality within sketches.
Some of them are abstract base classes, but in those cases there is at
least one reference example of a concrete class.

  * :class:`serde` is used when serializing and deserializing sketches.
  * :class:`jaccard` is used to compute the Jaccard similarity between pairs of theta or tuple sketches.
  * :class:`tuple_policy` is required to use a :class:`tuple_sketch` by specifying how summaries are combined.
  * :func:`ks_test` performs a Kolmogorov-Smirnov test on absolute-error quantiles family sketches.
  * :class:`kernel_function` is required when using a :class:`kernel_sketch` for Kernel Density Estimation.

.. toctree::
  :maxdepth: 1

  serde
  jaccard
  tuple_policy
  ks_test
  kernel
