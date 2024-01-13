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

   hyper_log_log
   cpc
   theta 
   tuple

Density Sketch
##############
.. toctree::
   :maxdepth: 1 

   density_sketch

Frequency Estimation
##########################

.. toctree::
   :maxdepth: 1 

   count_min_sketch


Frequent Items
##########################
This problem may also be known as **heavy hitters** or **TopK**

.. toctree::
   :maxdepth: 1 

   frequent_items

Quantile Estimation
###################

.. toctree::
   :maxdepth: 1 

   kll
   req
   quantiles_depr


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
