.. datasketches documentation master file, created by
   sphinx-quickstart on Tue Jul 25 11:04:59 2023.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Apache DataSketches
=================================================

**DataSketches** are highly-efficient algorithms to analyze big data quickly.
   
   
Counting Distincts
##################
..
   maxdepth: 1 means only the heading is printed in the contents
.. toctree::
   :maxdepth: 1

   distinct_counting/index

Quantile Estimation
###################

.. toctree::
   :maxdepth: 1 

   quantiles/index

Frequency Sketches
##################
This problem may also be known as **heavy hitters** or **TopK**

.. toctree::
   :maxdepth: 1 

   frequency/index

Vector Sketches
###############

.. toctree::
   :maxdepth: 1 

   vector/index


Random Sampling
###############

.. toctree::
   :maxdepth: 1

   varopt
   ebpps

Helper Classes
##############

.. toctree::
   :maxdepth: 1

   serde
   kernel
   jaccard
   tuple_policy
   ks_test


These classes are required for certain sketches or specific functionality withing sketches.
Some of them are abstract base classes, but in those cases there is at least one reference example of a 
concrete class.

   
.. note::

   This project is under active development.

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
