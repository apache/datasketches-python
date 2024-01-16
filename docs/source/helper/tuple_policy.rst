Tuple Policy
############

.. currentmodule:: datasketches

A Tuple Policy is needed when using a :class:`tuple_sketch` in order to specify how the 
summary values should be created, updated, combined, or intersected. A summary can consist of
any python object.

Each implementation must extend the abstract base class :class:`TuplePolicy`.

.. autoclass:: TuplePolicy
  
  .. automethod:: create_summary
  .. automethod:: update_summary
  .. automethod:: __call__

.. autoclass:: AccumulatorPolicy
  :show-inheritance:

.. autoclass:: MinIntPolicy
  :show-inheritance:

.. autoclass:: MaxIntPolicy
  :show-inheritance:
