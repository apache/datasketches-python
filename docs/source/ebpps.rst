Exact and Bounded, Probabilitiy Proportional to Size (EBPPS) Sampling
---------------------------------------------------------------------

.. currentmodule:: datasketches

An EBPPS sketch produces a randome sample of data from a stream of items, ensuring that the probability
of including an item is always exactly equal to the item's size. The size of an item is defined as its
weight relative to the total weight of all items seen so far by the sketch. In contrast to VarOpt sampling,
this sketch may return fewer than `k` items in order to keep the probability of including an item strictly
proportional to its size.

This sketch is based on: B. Hentschel, P. J. Haas, Y. Tian
"Exact PPS Sampling with Bounded Sample Size",
Information Processing Letters, 2023.

EBPPS sampling is related to reservoir sampling, but handles unequal item weights.
Feeding the sketch items with a uniform weight value will produce a sample equivalent to reservoir sampling.

.. note::
    Serializing and deserializing this sketch requires the use of a :class:`PyObjectSerDe`.

.. autoclass:: ebpps_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize

    .. rubric:: Static Methods:

    .. automethod:: deserialize

    .. rubric:: Non-static Methods:

    .. automethod:: __init__
