Theta Sketch
------------
The theta package contains the basic sketch classes that are members of the `Theta Sketch Framework <https://datasketches.apache.org/docs/Theta/ThetaSketchFramework.html>`_.
There is a separate Tuple package for many of the sketches that are derived from the same algorithms defined in the Theta Sketch Framework paper.

The *Theta Sketch* sketch is a space-efficient method for estimating cardinalities of sets.
It can also easily handle set operations (such as union, intersection, difference) while maintaining good accuracy.
Theta sketch is a practical variant of the K-Minimum Values sketch which avoids the need to sort the stored 
hash values on every insertion to the sketch.
It has better error properties than the HyperLogLog sketch for set operations beyond the simple union.

.. autoclass:: _datasketches.theta_sketch
    :members:
    :undoc-members: