Kernel Function
###############

.. currentmodule:: datasketches

A `kernel function <https://en.wikipedia.org/wiki/Positive-definite_kernel>`_ is a specific type of
mathematical funciton that is particularly useful in certain machine learning and pattern recognition
contexts. The :class:`density_sketch` performs approximate
`kernel density estimation <https://en.wikipedia.org/wiki/Kernel_density_estimation>`_ which, unsurprisingly,
relies on the use of such a kernel function.

The library provides an abstract base class :class:`KernelFunction` and an example implementation of a
Gaussian (also known as a Radial Basis Function) kernel. Custom classes must override the base class
and provide a floating point value as a score indicating the similarity of two input vectors.

.. autoclass:: KernelFunction
    
    .. automethod:: __call__

.. autoclass:: GaussianKernel
  :show-inheritance: