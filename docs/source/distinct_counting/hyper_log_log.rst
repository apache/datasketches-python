HyperLogLog (HLL)
-----------------
This is a high performance implementation of Phillipe Flajolet's HLL sketch but with significantly improved error behavior.

If the ONLY use case for sketching is counting uniques and merging, the HLL sketch is a reasonable choice, although the highest performing in terms of accuracy for storage space consumed is CPC (Compressed Probabilistic Counting). For large enough counts, this HLL version (with HLL_4) can be 2 to 16 times smaller than the Theta sketch family for the same accuracy.

This implementation offers three different types of HLL sketch, each with different trade-offs with accuracy, space and performance. 
These types are specified with the target_hll_type parameter.

In terms of accuracy, all three types, for the same lg_config_k, have the same error distribution as a function of ``n``, the number of unique values fed to the sketch.
The configuration parameter ``lg_config_k`` is the log-base-2 of ``k``, where ``k`` is the number of buckets or slots for the sketch.

During warmup, when the sketch has only received a small number of unique items (up to about 10% of ``k``), this implementation leverages a new class of estimator algorithms with significantly better accuracy.


.. autoclass:: _datasketches.tgt_hll_type

    .. autoattribute:: HLL_4
        :annotation: : 4 bits per entry

    .. autoattribute:: HLL_6
        :annotation: : 6 bits per entry

    .. autoattribute:: HLL_8
        :annotation: : 8 bits per entry


.. autoclass:: _datasketches.hll_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_max_updatable_serialization_bytes, get_rel_err 

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_max_updatable_serialization_bytes
    .. automethod:: get_rel_err

    .. rubric:: Non-static Methods:

    .. automethod:: __init__

.. autoclass:: _datasketches.hll_union
    :members:
    :undoc-members:
    :exclude-members: get_rel_err 

    .. rubric:: Static Methods:

    .. automethod:: get_rel_err

    .. rubric:: Non-static Methods:

    .. automethod:: __init__
    