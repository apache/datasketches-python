Distinct Counting 
=================

.. currentmodule:: datasketches

Distinct counting is one of the earliest tasks to which sketches were applied. The concept is simple:
Provide an estimate of the number of unique elements in a set of data. One of the earliest solutions came
from Flajolet and Martin in 1985 with their seminal work
`Probabilistic counting Algorithms for Data Base Applications <http://db.cs.berkeley.edu/cs286/papers/flajoletmartin-jcss1985.pdf>`_.

The DataSketches library offers several types of distinct counting sketches, each with different properties.

  * :class:`hll_sketch`: Hyper Log Log, a well-known sketch for distinct counting but no longer state-of-the-art.
  * :class:`cpc_sketch`: Provides a better accuracy-space trade-off than HLL, but with a somewhat larger footprint while in-memory.
  * :class:`theta_sketch`: Theta sketch, a type of k-minimum value sketch, which provide good performance with intersection and set difference operations.
  * :class:`tuple_sketch`: Tuple sketch, which is similar to a theta sketch but supports additional data stored with each key.

.. toctree::
  :maxdepth: 1
   
  hyper_log_log
  cpc
  theta 
  tuple