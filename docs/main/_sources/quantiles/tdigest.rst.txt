t-digest
--------

.. currentmodule:: datasketches

The implementation in this library is based on the MergingDigest described in
`Computing Extremely Accurate Quantiles Using t-Digests <https://arxiv.org/abs/1902.04023>`_ by Ted Dunning and Otmar Ertl.

The implementation in this library has a few differences from the reference implementation associated with that paper:

* Merge does not modify the input
* Derialization similar to other sketches in this library, although reading the reference implementation format is supported

Unlike all other algorithms in the library, t-digest is empirical and has no mathematical basis for estimating its error
and its results are dependent on the input data. However, for many common data distributions, it can produce excellent results.
t-digest also operates only on numeric data and, unlike the quantiles family algorithms in the library which return quantile
approximations from the input domain, t-digest interpolates values and will hold and return data points not seen in the input.

The closest alternative to t-digest in this library is REQ sketch. It prioritizes one chosen side of the rank domain:
either low rank accuracy or high rank accuracy. t-digest (in this implementation) prioritizes both ends of the rank domain
and has lower accuracy towards the middle of the rank domain (median).

Measurements show that t-digest is slightly biased (tends to underestimate low ranks and overestimate high ranks), while still
doing very well close to the extremes. The effect seems to be more pronounced with more input values.

For more information on the performance characteristics, see `the Datasketches page on t-digest <https://datasketches.apache.org/docs/tdigest/tdigest.html>`_.

.. autoclass:: tdigest_float
    :members:
    :undoc-members:
    :exclude-members: deserialize

    .. rubric:: Static Methods:

    .. automethod:: deserialize

    .. rubric:: Non-static Methods:

    .. automethod:: __init__

.. autoclass:: tdigest_double
    :members:
    :undoc-members:
    :exclude-members: deserialize

    .. rubric:: Static Methods:

    .. automethod:: deserialize

    .. rubric:: Non-static Methods:

    .. automethod:: __init__
