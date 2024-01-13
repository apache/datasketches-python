Quantiles Sketch (Deprecated)
-----------------------------

.. currentmodule:: datasketches

This is a deprecated quantiles sketch that is included for cross-language compatibility.
Most new projects will favor the KLL sketch over this one, or the REQ sketch for higher accuracy
at the very edge of a distribution.

This is a stochastic streaming sketch that enables near-real time analysis of the
approximate distribution from a very large stream in a single pass.
The analysis is obtained using `get_rank()` and `get_quantile()` functions,
the Probability Mass Function from `get_pmf()`` and the Cumulative Distribution Function from `get_cdf`.

Consider a large stream of one million values such as packet sizes coming into a network node.
The natural rank of any specific size value is its index in the hypothetical sorted
array of values.
The normalized rank is the natural rank divided by the stream size,
in this case one million.
The value corresponding to the normalized rank of `0.5` represents the 50th percentile or median
value of the distribution, or `get_quantile(0.5)`. 
Similarly, the 95th percentile is obtained from `get_quantile(0.95)`.

From the min and max values, for example, 1 and 1000 bytes,
you can obtain the PMF from `get_pmf(100, 500, 900)` that will result in an array of
4 fractional values such as {.4, .3, .2, .1}, which means that
40% of the values were < 100,
30% of the values were ≥ 100 and < 500,
20% of the values were ≥ 500 and < 900, and
10% of the values were ≥ 900.
A frequency histogram can be obtained by multiplying these fractions by `get_n()`,
which is the total count of values received.
The `get_cdf()`` works similarly, but produces the cumulative distribution instead.

As of November 2021, this implementation produces serialized sketches which are binary-compatible
with the equivalent Java implementation only when template parameter T = double
(64-bit double precision values).
 
The accuracy of this sketch is a function of the configured value `k`, which also affects
the overall size of the sketch. Accuracy of this quantile sketch is always with respect to
the normalized rank. A `k` of 128 produces a normalized, rank error of about 1.7%.
For example, the median item returned from `get_quantile(0.5)` will be between the actual items
from the hypothetically sorted array of input items at normalized ranks of 0.483 and 0.517, with
a confidence of about 99%.

.. note::
    For the :class:`quantiles_items_sketch`, objects must be comparable with ``__lt__``.

.. note::
    Serializing and deserializing a :class:`quantiles_items_sketch` requires the use of a :class:`PyObjectSerDe`.

.. autoclass:: quantiles_ints_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_normalized_rank_error

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_normalized_rank_error

    .. rubric:: Non-static Methods:

    .. automethod:: __init__

.. autoclass:: quantiles_floats_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_normalized_rank_error

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_normalized_rank_error

    .. rubric:: Non-static Methods:

    .. automethod:: __init__

.. autoclass:: quantiles_doubles_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_normalized_rank_error

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_normalized_rank_error

    .. rubric:: Non-static Methods:

    .. automethod:: __init__

.. autoclass:: quantiles_items_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_normalized_rank_error

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_normalized_rank_error

    .. rubric:: Non-static Methods:

    .. automethod:: __init__
