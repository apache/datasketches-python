Quantiles Sketches
==================

Quantile estimation is useful for understanding the distribution of data values in a stream. The sketches currently
in the library are designed to answer queries about the `rank` of an item in the stream of items. That is, when
applying a global ordering on all the items, what is the portion of items seen so far that are less than (alternatively,
less-than-or-equal-to) the given item. Using straightforward logic, they can also estimate the item at a given rank
in the stream.

These sketches may be used to compute approximate histograms, Probability Mass Functions (PMFs), or
Cumulative Distribution Functions (CDFs).

The library provides three types of quantiles sketches, each of which has generic items as well as versions
specific to a given numeric type (e.g. integer or floating point values). All three types provide error
bounds on rank estimation with proven probabilistic error distributions.

  * KLL: Provides uniform rank estimation error over the entire range
  * REQ: Provides relative rank error estimates, which decreases approaching either the high or low end values.
  * Classic quantiles: Largely deprecated in favor of KLL, also provides uniform rank estimation error. Included largely for backwards compatibility with historic data.

.. toctree::
  :maxdepth: 1
   
  kll
  req
  quantiles_depr