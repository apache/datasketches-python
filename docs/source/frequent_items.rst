Frequent Items
--------------

.. currentmodule:: datasketches

This sketch is useful for tracking approximate frequencies of items (``object`` or ``string``) with optional associated
integer counts that are members of a multiset of such items. 
The true frequency of an item is defined to be the sum of associated counts.

This implementation provides the following capabilities:

* Estimate the *frequency* of an item.
* Return *upper* and *lower bounds* of any item, such that the true frequency is always between the upper and lower bounds.
* Return a global *maximum error* that holds for all items in the stream.
* Return an array of frequent items that qualify either a *NO_FALSE_POSITIVES* or a *NO_FALSE_NEGATIVES* error type.
* *Merge* itself with another sketch object created from this class.
* *Serialize/Deserialize* to/from a byte array.

**Space Usage**

The sketch is initialized with a maximum map size, ``maxMapSize``, that specifies the maximum physical length of the internal hash map of the form ``(object item, int count)``. 
The maximum map size is always a power of 2, defined through the variables ``lg_max_map_size``.

The hash map starts at a very small size (8 entries) and grows as needed up to the specified maximum map size.

Excluding external space required for the item objects, the internal memory space usage of this sketch is ``18 * mapSize`` bytes (assuming 8 bytes for each reference), 
plus a small constant number of additional bytes. 
The internal memory space usage of this sketch will never exceed ``18 * maxMapSize`` bytes, plus a small constant number of additional bytes.

**Maximum Capacity of the Sketch**

The ``LOAD_FACTOR`` for the hash map is internally set at :math:`75\%`, which means at any time the map capacity of ``(item, count)`` pairs is ``mapCap = 0.75 * mapSize``. 
The maximum capacity of ``(item, count)`` pairs of the sketch is ``maxMapCap = 0.75 * maxMapSize``.

**Updating the sketch with ``(item, count)`` pairs**

If the item is found in the hash map, the mapped count field (the "counter") is incremented by the incoming count; otherwise, a new counter ``(item, count)`` pair is created. 
If the number of tracked counters reaches the maximum capacity of the hash map, the sketch decrements all of the counters (by an approximately computed median) 
and removes any non-positive counters.

**Accuracy**

If fewer than ``0.75 * maxMapSize`` different items are inserted into the sketch, the estimated frequencies returned by the sketch will be exact.

The logic of the frequent items sketch is such that the stored counts and true counts are never too different. 
More specifically, for any item, the sketch can return an estimate of the true frequency of item, along with upper and lower bounds on the frequency (that hold deterministically).

For this implementation and for a specific active item, it is guaranteed that the true frequency will be between the Upper Bound (UB) and the Lower Bound (LB) computed for that item. 
Specifically, ``(UB- LB) ≤ W * epsilon``, where :math:`W` denotes the sum of all item counts, and :math:`epsilon = 3.5/M`, where :math:`epsilon = M` is the maxMapSize.

This is a worst-case guarantee that applies to arbitrary inputs.
For inputs typically seen in practice, ``(UB-LB)`` is usually much smaller.

**Background**

This code implements a variant of what is commonly known as the "Misra-Gries algorithm". 
Variants of it were discovered and rediscovered and redesigned several times over the years:

* *Finding repeated elements*, Misra, Gries, 1982
* *Frequency estimation of Internet packet streams with limited space* Demaine, Lopez-Ortiz, Munro, 2002
* *A simple algorithm for finding frequent elements in streams and bags* Karp, Shenker, Papadimitriou, 2003
* *Efficient Computation of Frequent and Top-k Elements in Data Streams* Metwally, Agrawal, Abbadi, 2006


For speed, we do employ some randomization that introduces a small probability that our proof of the worst-case bound might not apply to a given run. 
However, we have ensured that this probability is extremely small. 
For example, if the stream causes one table purge (rebuild), our proof of the worst-case bound applies with a probability of at least `1 - 1E-14`. 
If the stream causes ``1E9`` purges, our proof applies with a probability of at least ``1 - 1E-5``.

There are two flavors of Frequent Items Sketches, one with generic items (objects) and another specific to strings.
The string version is a legacy name from before the library supported generic objects and is retained
only for backwards compatibility.

.. note::
    The :class:`frequent_items_sketch` uses an input object's ``__hash__`` and ``__eq__`` methods.

.. note::
    Serializing and deserializing the :class:`frequent_items_sketch` requires the use of a :class:`PyObjectSerDe`.

.. autoclass:: frequent_items_error_type

    .. autoattribute:: NO_FALSE_POSITIVES
        :annotation: : Returns only true positives but may miss some heavy hitters.

    .. autoattribute:: NO_FALSE_NEGATIVES
        :annotation: : Does not miss any heavy hitters but may return false positives.


.. autoclass:: frequent_items_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_epsilon_for_lg_size, get_apriori_error
    :member-order: groupwise

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_epsilon_for_lg_size
    .. automethod:: get_apriori_error

    .. rubric:: Non-static Methods:

    .. automethod:: __init__


.. autoclass:: frequent_strings_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_epsilon_for_lg_size, get_apriori_error
    :member-order: groupwise

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_epsilon_for_lg_size
    .. automethod:: get_apriori_error

    .. rubric:: Non-static Methods:

    .. automethod:: __init__
