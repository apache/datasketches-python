Relative Error Quantiles (REQ) Sketch
-------------------------------------

.. currentmodule:: datasketches

This is an implementation based on the `paper <https://arxiv.org/abs/2004.01668>`_ "Relative Error Streaming Quantiles" by Graham Cormode, Zohar Karnin, Edo Liberty, Justin Thaler, Pavel Veselý, and loosely derived from a Python prototype written by Pavel Veselý.

This implementation differs from the algorithm described in the paper in the following:

The algorithm requires no upper bound on the stream length. 
Instead, each relative-compactor counts the number of compaction operations performed so far (via variable state). 
Initially, the relative-compactor starts with `INIT_NUMBER_OF_SECTIONS`. 
Each time the number of compactions `(variable state) exceeds 2^{numSections - 1}`, we double `numSections`. 
Note that after merging the sketch with another one variable state may not correspond to the number of compactions performed at a particular level, however, 
since the state variable never exceeds the number of compactions, the guarantees of the sketch remain valid.

The size of each section (variable `k` and `section_size` in the code and parameter `k` in the paper) is 
initialized with a number set by the user via variable `k`. 
When the number of sections doubles, we decrease section_size by a factor of `sqrt(2)`. 
This is applied at each level separately. 
Thus, when we double the number of sections, the nominal compactor size increases by a factor of approx. `sqrt(2) (+/- rounding)`.

The merge operation here does not perform "special compactions", which are used in the paper to allow for a tight mathematical analysis of the sketch.
This implementation provides a number of capabilities not discussed in the paper or provided in the Python prototype.

The Python prototype only implemented high accuracy for low ranks. This implementation provides the user with the ability to 
choose either high rank accuracy or low rank accuracy at the time of sketch construction.
The Python prototype only implemented a comparison criterion of `INCLUSIVE`. 
This implementation allows the user to use both the `INCLUSIVE` criterion and the `EXCLUSIVE` criterion.
This implementation provides extensive debug visibility into the operation of the sketch with two levels of detail output. 
This is not only useful for debugging, but is a powerful tool to help users understand how the sketch works.

.. note::
    For the :class:`req_items_sketch`, objects must be comparable with ``__lt__``.

.. note::
    Serializing and deserializing a :class:`req_items_sketch` requires the use of a :class:`PyObjectSerDe`.

.. autoclass:: req_ints_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_RSE

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_RSE

    .. rubric:: Non-static Methods:

    .. automethod:: __init__

.. autoclass:: req_floats_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_RSE

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_RSE

    .. rubric:: Non-static Methods:

    .. automethod:: __init__

.. autoclass:: req_items_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_RSE

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_RSE

    .. rubric:: Non-static Methods:

    .. automethod:: __init__
