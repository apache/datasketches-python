Theta Sketch
------------

.. currentmodule:: datasketches

Theta sketches are used for distinct counting.

The theta package contains the basic sketch classes that are members of the `Theta Sketch Framework <https://datasketches.apache.org/docs/Theta/ThetaSketchFramework.html>`_.
There is a separate Tuple package for many of the sketches that are derived from the same algorithms defined in the Theta Sketch Framework paper.

The *Theta Sketch* sketch is a space-efficient method for estimating cardinalities of sets.
It can also easily handle set operations (such as union, intersection, difference) while maintaining good accuracy.
Theta sketch is a practical variant of the K-Minimum Values sketch which avoids the need to sort the stored 
hash values on every insertion to the sketch.
It has better error properties than the HyperLogLog sketch for set operations beyond the simple union.

Set operations (union, intersection, A-not-B) are performed through the use of dedicated objects.

Several `Jaccard similarity <https://en.wikipedia.org/wiki/Jaccard_similarity>`_
measures can be computed between theta sketches with the :class:`theta_jaccard_similarity` class.

.. autoclass:: theta_sketch
    :members:
    :undoc-members:
    
.. autoclass:: update_theta_sketch
    :members:
    :undoc-members:

    .. automethod:: __init__


.. autoclass:: compact_theta_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize

    .. rubric:: Static Methods:
        
    .. automethod:: deserialize

    .. rubric:: Non-static Methods:

    .. automethod:: __init__


.. autoclass:: theta_union
    :members:
    :undoc-members:

    .. automethod:: __init__


.. autoclass:: theta_intersection
    :members:
    :undoc-members:

    .. automethod:: __init__


.. autoclass:: theta_a_not_b
    :members:
    :undoc-members:

    .. automethod:: __init__
