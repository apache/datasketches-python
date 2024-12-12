CountMin Sketch
---------------

The CountMin sketch, as described in Cormode and Muthukrishnan in
http://dimacs.rutgers.edu/~graham/pubs/papers/cm-full.pdf,
is used for approximate Frequency Estimation. 
For an item :math:`x` with frequency :math:`f_x`, the sketch provides an estimate, :math:`\hat{f_x}`, 
such that :math:`f_x \approx \hat{f_x}.` 
The sketch guarantees that :math:`f_x \le \hat{f_x}` and provides a probabilistic upper bound which is dependent on the size parameters.
The sketch provides an estimate of the occurrence frequency for any queried item but, in contrast
to the Frequent Items Sketch, this sketch does not provide a list of 
heavy hitters.

.. currentmodule:: _datasketches

.. autoclass:: count_min_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, suggest_num_buckets, suggest_num_hashes

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: suggest_num_buckets
    .. automethod:: suggest_num_hashes

    .. rubric:: Non-static Methods:
