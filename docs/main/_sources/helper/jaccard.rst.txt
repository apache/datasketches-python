Jaccard Similarity
##################

.. currentmodule:: datasketches

These objects provide measures related to the `Jaccard similarity <https://en.wikipedia.org/wiki/Jaccard_similarity>`_
of :class:`theta_sketch` and :class:`tuple_sketch` objects.

Note that there are separate classes to be used for theta and tuple sketches.

.. autoclass:: theta_jaccard_similarity

  .. automethod:: jaccard
  .. automethod:: exactly_equal
  .. automethod:: similarity_test
  .. automethod:: dissimilarity_test    

.. autoclass:: tuple_jaccard_similarity

  .. automethod:: jaccard
  .. automethod:: exactly_equal
  .. automethod:: similarity_test
  .. automethod:: dissimilarity_test    
