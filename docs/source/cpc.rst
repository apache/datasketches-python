Compressed Probabilistic Counting (CPC)
---------------------------------------
High performance C++ implementation of Compressed Probabilistic Counting (CPC) Sketch

This is a very compact (in serialized form) distinct counting sketch.
The theory is described in the following paper:
https://arxiv.org/abs/1708.06839

.. autoclass:: _datasketches.cpc_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize,
    :member-order: groupwise

    .. rubric:: Static Methods:

    .. automethod:: deserialize

    .. rubric:: Non-static Methods:

