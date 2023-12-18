/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <nanobind/nanobind.h>
#include <nanobind/trampoline.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/vector.h>

#include <numpy/arrayobject.h>

#ifndef _KERNEL_FUNCTION_HPP_
#define _KERNEL_FUNCTION_HPP_

namespace nb = nanobind;

namespace datasketches {

/**
 * @brief kernel_function provides the underlying base class from
 *        which native Python kernels ultimately inherit. The actual
 *        kernels implement KernelFunction, as shown in KernelFunction.py
 */
struct kernel_function {
  //virtual double operator()(const nb::ndarray<nb::numpy, double>& a, const nb::ndarray<nb::numpy, double>& b) const = 0;
  virtual double operator()(PyObject* a, PyObject* b) const = 0;
  virtual ~kernel_function() = default;
};

/**
 * @brief KernelFunction provides the "trampoline" class for pybind11
 *        that allows for a native Python implementation of kernel
 *        functions.
 */
struct KernelFunction : public kernel_function {
  NB_TRAMPOLINE(kernel_function, 1);

  /**
   * @brief Evaluates K(a,b), the kernel function for the given points a and b
   * @param a the first vector
   * @param b the second vector
   * @return The function value K(a,b)
   */
  //double operator()(const nb::ndarray<nb::numpy, double>& a, const nb::ndarray<nb::numpy, double>& b) const override {
  double operator()(PyObject* a, PyObject* b) const override {
    NB_OVERRIDE_PURE_NAME(
      "__call__",      // Name of function in python
      operator(),      // Name of function in C++
      a, b             // Arguments
    );
  }
};

/* The kernel_function_holder provides a concrete class that dispatches calls
 * from the sketch to the kernel_function. This class is needed to provide a
 * concrete object to produce a compiled library, but library users should
 * never need to use this directly.
 */
struct kernel_function_holder {
  explicit kernel_function_holder(std::shared_ptr<kernel_function> kernel) : _kernel(kernel) {}
  kernel_function_holder(const kernel_function_holder& other) : _kernel(other._kernel) {}
  kernel_function_holder(kernel_function_holder&& other) : _kernel(std::move(other._kernel)) {}
  kernel_function_holder& operator=(const kernel_function_holder& other) { _kernel = other._kernel; return *this; }
  kernel_function_holder& operator=(kernel_function_holder&& other) { std::swap(_kernel, other._kernel); return *this; }

  double operator()(const std::vector<double>& a, PyObject* b) const {
    npy_intp size_a = a.size();
    nb::handle a_obj(PyArray_SimpleNewFromData(1, &size_a, NPY_DOUBLE, const_cast<double*>(a.data())));
    return _kernel->operator()(
      a_obj.ptr(),
      b
    );
  }

  double operator()(const std::vector<double>& a, const std::vector<double>& b) const {
    npy_intp size_a = a.size();
    npy_intp size_b = b.size();
    nb::handle a_obj(PyArray_SimpleNewFromData(1, &size_a, NPY_DOUBLE, const_cast<double*>(a.data())));
    nb::handle b_obj(PyArray_SimpleNewFromData(1, &size_b, NPY_DOUBLE, const_cast<double*>(b.data())));
    return _kernel->operator()(
      a_obj.ptr(),
      b_obj.ptr()
    );
  }

  private:
    std::shared_ptr<kernel_function> _kernel;
};

}

#endif // _KERNEL_FUNCTION_HPP_
