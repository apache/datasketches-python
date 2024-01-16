Variance Optimal Sampling (VarOpt)
----------------------------------

.. currentmodule:: datasketches

A VarOpt sketch samples data from a stream of items. The sketch is desinged for optimal (minimum)
variance when querying the sketch to estimate subset sums of items matching a provided predicate.
The sketch will produce a sample of size `k` (or smaller if fewer items have been presented), with
the probability of including an item roughly corresponding it the item's weight relative to the total
weight of all items presented to the sketch.

VarOpt sampling is related to reservoir sampling, with improved error bounds for subset sum estimation.
Feeding the sketch items with a uniform weight value will produce a sample equivalent to reservoir sampling.

.. note::
    Serializing and deserializing this sketch requires the use of a :class:`PyObjectSerDe`.

.. autoclass:: var_opt_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize
    
    .. rubric:: Static Methods:

    .. automethod:: deserialize

    .. rubric:: Non-static Methods:

    .. automethod:: __init__

.. autoclass:: var_opt_union
    :members:
    :undoc-members:
    :exclude-members: deserialize
    
    .. rubric:: Static Methods:

    .. automethod:: deserialize

    .. rubric:: Non-static Methods:

    .. automethod:: __init__
