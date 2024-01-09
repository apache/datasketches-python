HyperLogLog (HLL)
-----------------
This is a high performance implementation of Phillipe Flajolet's HLL sketch but with significantly improved error behavior.

If the ONLY use case for sketching is counting uniques and merging, the HLL sketch is a reasonable choice, although the highest performing in terms of accuracy for storage space consumed is CPC (Compressed Probabilistic Counting). For large enough counts, this HLL version (with HLL_4) can be 2 to 16 times smaller than the Theta sketch family for the same accuracy.

This implementation offers three different types of HLL sketch, each with different trade-offs with accuracy, space and performance. 
These types are specified with the target_hll_type parameter.

In terms of accuracy, all three types, for the same lg_config_k, have the same error distribution as a function of n, the number of unique values fed to the sketch.
The configuration parameter `lg_config_k`` is the log-base-2 of `K`, where `K`` is the number of buckets or slots for the sketch.

During warmup, when the sketch has only received a small number of unique items (up to about 10% of `K`), this implementation leverages a new class of estimator algorithms with significantly better accuracy.

This sketch also offers the capability of operating off-heap. 
Given a WritableMemory object created by the user, the sketch will perform all of its updates and internal phase transitions in that object, which can actually reside either on-heap or off-heap based on how it is configured. 
In large systems that must update and merge many millions of sketches, having the sketch operate off-heap avoids the serialization and deserialization costs of moving sketches to and from off-heap memory-mapped files, for example, and eliminates big garbage collection delays.

.. autoclass:: _datasketches.hll_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_max_updatable_serialization_bytes, get_rel_err 

    :member-order: groupwise

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_max_updatable_serialization_bytes
    .. automethod:: get_rel_err

    .. rubric:: Non-static Methods:
