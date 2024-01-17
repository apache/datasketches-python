Tuple Sketch
------------

.. currentmodule:: datasketches

Tuple sketches are an extension of Theta sketches, meaning they provide estimate of distinct counts, that
allow the keeping of arbitrary summaries associated with each retained key 
(for example, a count for every key). The use of a :class:`tuple_sketch` requires a :class:`TuplePolicy` which
defines how summaries are created, updated, merged, or intersected. The library provides a few basic 
examples of :class:`TuplePolicy` implementations, but the right custom summary and policy can allow very
complicated analysis to be performed quite easily.

Set operations (union, intersection, A-not-B) are performed through the use of dedicated objects.

Several `Jaccard similarity <https://en.wikipedia.org/wiki/Jaccard_similarity>`_
measures can be computed between theta sketches with the :class:`tuple_jaccard_similarity` class.

.. note::
    Serializing and deserializing this sketch requires the use of a :class:`PyObjectSerDe`.

.. autoclass:: tuple_sketch
    :members:
    :undoc-members:
    
.. autoclass:: update_tuple_sketch
    :members:
    :undoc-members:

    .. automethod:: __init__


.. autoclass:: compact_tuple_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize

    .. rubric:: Static Methods:
        
    .. automethod:: deserialize

    .. rubric:: Non-static Methods:

    .. automethod:: __init__


.. autoclass:: tuple_union
    :members:
    :undoc-members:

    .. automethod:: __init__


.. autoclass:: tuple_intersection
    :members:
    :undoc-members:

    .. automethod:: __init__


.. autoclass:: tuple_a_not_b
    :members:
    :undoc-members:

    .. automethod:: __init__
