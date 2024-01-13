Density Sketch
--------------

.. currentmodule:: datasketches

Builds a coreset from the given set of input points. 
Provides density estimate at a given point.

Based on the following paper: Zohar Karnin, Edo Liberty 
"Discrepancy, Coresets, and Sketches in Machine Learning" 
https://proceedings.mlr.press/v99/karnin19a/karnin19a.pdf

Inspired by the following implementation: https://github.com/edoliberty/streaming-quantiles/blob/f688c8161a25582457b0a09deb4630a81406293b/gde.py

Requires the use of a :class:`KernelFunction` to compute the distance between two vectors.

.. autoclass:: density_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize

    .. rubric:: Static Methods:

    .. automethod:: deserialize

    .. rubric:: Non-static Methods:

    .. automethod:: __init__
