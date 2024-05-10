Serialize/Deserialize (SerDe)
#############################

.. currentmodule:: datasketches

A SerDe is a class used to serialize items sketches to a :class:`bytes` object in binary.
Several example SerDes are provided as references.

The use of binary-compatible SerDes in different languages is critical for cross-language compatibility.

Each implementation must extend the :class:`PyObjectSerDe` class and override all three of its methods.

.. autoclass:: PyObjectSerDe
  
  .. automethod:: get_size
  .. automethod:: to_bytes
  .. automethod:: from_bytes


The provided SerDes are:

.. autoclass:: PyStringsSerDe
  :show-inheritance:

.. autoclass:: PyIntsSerDe
  :show-inheritance:

.. autoclass:: PyLongsSerDe
  :show-inheritance:

.. autoclass:: PyFloatsSerDe
  :show-inheritance:

.. autoclass:: PyDoublesSerDe
