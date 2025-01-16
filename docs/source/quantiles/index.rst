Quantiles Sketches
==================

Quantile estimation is useful for understanding the distribution of data values in a stream. The sketches currently
in the library are designed to answer queries about the `rank` of an item in the stream of items. That is, when
applying a global ordering on all the items, what is the portion of items seen so far that are less than (alternatively,
less-than-or-equal-to) the given item. Using straightforward logic, they can also estimate the item at a given rank
in the stream.

These sketches may be used to compute approximate histograms, Probability Mass Functions (PMFs), or
Cumulative Distribution Functions (CDFs).

The library provides four types of quantiles sketches, three of which have generic items as well as versions
specific to a given numeric type (e.g. integer or floating point values). Those three types provide error
bounds on rank estimation with proven probabilistic error distributions. t-digest is a heuristic-based sketch
that works only on numeric data, and while the error properties are not guaranteed, the sketch typically
does a good job with small storage.

  * KLL: Provides uniform rank estimation error over the entire range.
  * REQ: Provides relative rank error estimates, which decreases approaching either the high or low end values.
  * t-digest: Relative rank error estimates, heuristic-based without guarantees but quite compact with generally very good error properties.
  * Classic quantiles: Largely deprecated in favor of KLL, also provides uniform rank estimation error. Included largely for backwards compatibility with historic data.

.. toctree::
  :maxdepth: 1

  kll
  req
  tdigest
  quantiles_depr