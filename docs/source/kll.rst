KLL Sketch
----------

.. currentmodule:: datasketches

Implementation of a very compact quantiles sketch with lazy compaction scheme
and nearly optimal accuracy per retained item.
See `Optimal Quantile Approximation in Streams`.

This is a stochastic streaming sketch that enables near real-time analysis of the
approximate distribution of items from a very large stream in a single pass, requiring only
that the items are comparable.
The analysis is obtained using `get_quantile()` function or the
inverse functions `get_rank()`, `get_pmf()` (Probability Mass Function), and `get_cdf()`
(Cumulative Distribution Function).

As of May 2020, this implementation produces serialized sketches which are binary-compatible
with the equivalent Java implementation only when template parameter `T = float`
(32-bit single precision values).

Given an input stream of `N` items, the `natural rank` of any specific
item is defined as its index `(1 to N)` in inclusive mode
or `(0 to N-1)` in exclusive mode
in the hypothetical sorted stream of all `N` input items.

The `normalized rank` (`rank`) of any specific item is defined as its
`natural rank` divided by `N`.
Thus, the `normalized rank` is between zero and one.
In the documentation for this sketch `natural rank` is never used so any
reference to just `rank` should be interpreted to mean `normalized rank`.

This sketch is configured with a parameter `k`, which affects the size of the sketch
and its estimation error.

The estimation error is commonly called `epsilon` (or `eps`) and is a fraction
between zero and one. Larger values of `k` result in smaller values of `epsilon`.
Epsilon is always with respect to the rank and cannot be applied to the
corresponding items.

The relationship between the `normalized rank` and the corresponding items can be viewed
as a two-dimensional monotonic plot with the `normalized rank` on one axis and the
corresponding items on the other axis. If the y-axis is specified as the item-axis and
the x-axis as the `normalized rank`, then `y = get_quantile(x)` is a monotonically
increasing function.

The function `get_quantile(rank)` translates ranks into
corresponding quantiles. The functions `get_rank(item)`,
`get_cdf(...)` (Cumulative Distribution Function), and `get_pmf(...)`
(Probability Mass Function) perform the opposite operation and translate items into ranks.

The `get_pmf(...)` function has about 13 to 47% worse rank error (depending
on `k`) than the other queries because the mass of each "bin" of the PMF has
"double-sided" error from the upper and lower edges of the bin as a result of a subtraction,
as the errors from the two edges can sometimes add.

The default `k` of 200 yields a "single-sided" `epsilon` of about 1.33% and a
"double-sided" (PMF) `epsilon` of about 1.65%.

A `get_quantile(rank)` query has the following guarantees:
- Let `q = get_quantile(r)` where `r` is the rank between zero and one.
- The quantile `q` will be an item from the input stream.
- Let `true_rank` be the true rank of `q` derived from the hypothetical sorted
stream of all `N` items.
- Let `eps = get_normalized_rank_error(false)`.
- Then `r - eps ≤ true_rank ≤ r + eps` with a confidence of 99%. Note that the
error is on the rank, not the quantile.

A `get_rank(item)` query has the following guarantees:
- Let `r = get_rank(i)` where `i` is an item between the min and max items of
the input stream.
- Let `true_rank` be the true rank of `i` derived from the hypothetical sorted
stream of all `N` items.
- Let `eps = get_normalized_rank_error(false)`.
- Then `r - eps ≤ true_rank ≤ r + eps` with a confidence of 99%.

A `get_pmf()` query has the following guarantees:
- Let `{r1, r2, ..., r(m+1)} = get_pmf(s1, s2, ..., sm)` where `s1, s2` are
split points (items from the input domain) between the min and max items of
the input stream.
- Let `mass_i = estimated mass between s_i and s_i+1`.
- Let `true_mass` be the true mass between the items of `s_i`,
`s_i+1` derived from the hypothetical sorted stream of all `N` items.
- Let `eps = get_normalized_rank_error(true)`.
- then `mass - eps ≤ true_mass ≤ mass + eps` with a confidence of 99%.
- `r(m+1)` includes the mass of all points larger than `s_m`.

A `get_cdf(...)` query has the following guarantees;
- Let `{r1, r2, ..., r(m+1)} = get_cdf(s1, s2, ..., sm)` where `s1, s2, ...` are
split points (items from the input domain) between the min and max items of
the input stream.
- Let `mass_i = r_(i+1) - r_i`.
- Let `true_mass` be the true mass between the true ranks of `s_i`,
`s_i+1` derived from the hypothetical sorted stream of all `N` items.
- Let `eps = get_normalized_rank_error(true)`.
- then `mass - eps ≤ true_mass ≤ mass + eps` with a confidence of 99%.
- `1 - r(m+1)` includes the mass of all points larger than `s_m`.

From the above, it might seem like we could make some estimates to bound the
`item` returned from a call to `get_quantile()`. The sketch, however, does not
let us derive error bounds or confidences around items. Because errors are independent, we
can approximately bracket a value as shown below, but there are no error estimates available.
Additionally, the interval may be quite large for certain distributions.
- Let `q = get_quantile(r)`, the estimated quantile of rank `r`.
- Let `eps = get_normalized_rank_error(false)`.
- Let `q_lo = estimated quantile of rank (r - eps)`.
- Let `q_hi = estimated quantile of rank (r + eps)`.
- Then `q_lo ≤ q ≤ q_hi`, with 99% confidence.

.. note::
    For the :class:`kll_items_sketch`, objects must be comparable with ``__lt__``.

.. note::
    Serializing and deserializing a :class:`kll_items_sketch` requires the use of a :class:`PyObjectSerDe`.


.. autoclass:: kll_ints_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_normalized_rank_error

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_normalized_rank_error

    .. rubric:: Non-static Methods:

    .. automethod:: __init__

.. autoclass:: kll_floats_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_normalized_rank_error

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_normalized_rank_error

    .. rubric:: Non-static Methods:

    .. automethod:: __init__

.. autoclass:: kll_doubles_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_normalized_rank_error

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_normalized_rank_error

    .. rubric:: Non-static Methods:

    .. automethod:: __init__

.. autoclass:: kll_items_sketch
    :members:
    :undoc-members:
    :exclude-members: deserialize, get_normalized_rank_error

    .. rubric:: Static Methods:

    .. automethod:: deserialize
    .. automethod:: get_normalized_rank_error

    .. rubric:: Non-static Methods:

    .. automethod:: __init__

