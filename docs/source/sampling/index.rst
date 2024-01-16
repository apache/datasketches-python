Random Sampling Sketches
========================

.. currentmodule:: datasketches

These sketches are used to randomly sample items. The length of the input
stream does not need to be known in advance.

Both VarOpt (Variance Optimal) and EBPPS (Exact and Bounded,
Probability Proportional to Size) sketches will include sample items based on
each item's weight relative to the weight of the entire stream but
they differ in details:

  * :class:`ebpps_sketch` ensures that the probability of including an item is always exactly proportional to the item's weight.
  * :class:`var_opt_sketch` optimizes for applying a predicate to the resulting sample such that the variance of the subset sum after applying the predicate is minimized, even if the inclusion probability differs somewhat from being proportional to the item's weight.

.. toctree::
  :maxdepth: 1

  varopt
  ebpps
