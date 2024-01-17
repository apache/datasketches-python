Compressed Probabilistic Counting (CPC)
---------------------------------------
High performance C++ implementation of Compressed Probabilistic Counting (CPC) Sketch.
This is a unique-counting sketch that implements the Compressed Probabilistic Counting (CPC, a.k.a FM85) algorithms developed by Kevin Lang in his paper
`Back to the Future: an Even More Nearly Optimal Cardinality Estimation Algorithm <https://arxiv.org/abs/1708.06839>`_.
This sketch is extremely space-efficient when serialized. 
In an apples-to-apples empirical comparison against compressed HyperLogLog sketches, this new algorithm simultaneously wins on the two dimensions of the space/accuracy tradeoff and produces sketches that are smaller than the entropy of HLL, so no possible implementation of compressed HLL can match its space efficiency for a given accuracy. As described in the paper this sketch implements a newly developed ICON estimator algorithm that survives unioning operations, another well-known estimator, the Historical Inverse Probability (HIP) estimator does not. 
The update speed performance of this sketch is quite fast and is comparable to the speed of HLL. 
The unioning (merging) capability of this sketch also allows for merging of sketches with different configurations of K.
For additional security this sketch can be configured with a user-specified hash seed.


.. autoclass:: _datasketches.cpc_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize

    .. rubric:: Static Methods:

    .. automethod:: deserialize

    .. rubric:: Non-static Methods:

    .. automethod:: __init__


.. autoclass:: _datasketches.cpc_union
    :members:
    :undoc-members:
    :exclude-members: deserialize

    .. automethod:: __init__
