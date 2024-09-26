Frequency Sketches
==================

Frequency estimation involves determining how often an item has been seen in a stream. The library currently
offers two types of sketches for frequency estimation, one of which has two closely-related variants.

  * :class:`frequent_items_sketch`: Identifies the *Top K* or *heavy hitters* in a stream, those items whose weight is above a certain percentage of the entire stream. Does not necessarily provide an estimate for most items outside the heavy hitters.
  * :class:`frequent_strings_sketch`: Like the items version but containing snly strings (an implementation from before the library handled generic objects).
  * :class:`count_min_sketch`: Provides an estimate for any item, regardless of relative weight, but does not maintain a list of the heaviest items.

.. toctree::
  :maxdepth: 1
   
  frequent_items
  count_min_sketch