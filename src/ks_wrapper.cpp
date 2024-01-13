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

#include "kolmogorov_smirnov.hpp"
#include "kll_sketch.hpp"
#include "quantiles_sketch.hpp"
#include "py_object_lt.hpp"

#include <nanobind/nanobind.h>

namespace nb = nanobind;

void init_kolmogorov_smirnov(nb::module_ &m) {
  using namespace datasketches;

  m.def("ks_test", &kolmogorov_smirnov::test<kll_sketch<int>>, nb::arg("sk_1"), nb::arg("sk_2"), nb::arg("p"),
    "Performs the Kolmogorov-Smirnov Test for :code:`kll_ints_sketch` pairs.\n"
    "Note: if the given sketches have insufficient data or if the sketch sizes are too small, "
    "this will return false.\n"
    "Returns True if we can reject the null hypothesis (that the sketches reflect the same underlying "
    "distribution) using the provided p-value, otherwise False.");
  m.def("ks_test", &kolmogorov_smirnov::test<kll_sketch<float>>, nb::arg("sk_1"), nb::arg("sk_2"), nb::arg("p"),
    "Performs the Kolmogorov-Smirnov Test for :code:`kll_floats_sketch` pairs.\n"
    "Note: if the given sketches have insufficient data or if the sketch sizes are too small, "
    "this will return false.\n"
    "Returns True if we can reject the null hypothesis (that the sketches reflect the same underlying "
    "distribution) using the provided p-value, otherwise False.");
  m.def("ks_test", &kolmogorov_smirnov::test<kll_sketch<double>>, nb::arg("sk_1"), nb::arg("sk_2"), nb::arg("p"),
    "Performs the Kolmogorov-Smirnov Test for :code:`kll_doubles_sketch` pairs.\n"
    "Note: if the given sketches have insufficient data or if the sketch sizes are too small, "
    "this will return false.\n"
    "Returns True if we can reject the null hypothesis (that the sketches reflect the same underlying "
    "distribution) using the provided p-value, otherwise False.");
  m.def("ks_test", &kolmogorov_smirnov::test<kll_sketch<nb::object, py_object_lt>>, nb::arg("sk_1"), nb::arg("sk_2"), nb::arg("p"),
    "Performs the Kolmogorov-Smirnov Test for :code:`kll_items_sketch` pairs.\n"
    "Note: if the given sketches have insufficient data or if the sketch sizes are too small, "
    "this will return false.\n"
    "Returns True if we can reject the null hypothesis (that the sketches reflect the same underlying "
    "distribution) using the provided p-value, otherwise False.");

  m.def("ks_test", &kolmogorov_smirnov::test<quantiles_sketch<int>>, nb::arg("sk_1"), nb::arg("sk_2"), nb::arg("p"),
    "Performs the Kolmogorov-Smirnov Test for :code:`quantiles_ints_sketch` pairs.\n"
    "Note: if the given sketches have insufficient data or if the sketch sizes are too small, "
    "this will return false.\n"
    "Returns True if we can reject the null hypothesis (that the sketches reflect the same underlying "
    "distribution) using the provided p-value, otherwise False.");
  m.def("ks_test", &kolmogorov_smirnov::test<quantiles_sketch<float>>, nb::arg("sk_1"), nb::arg("sk_2"), nb::arg("p"),
    "Performs the Kolmogorov-Smirnov Test for :code:`quantiles_floats_sketch` pairs.\n"
    "Note: if the given sketches have insufficient data or if the sketch sizes are too small, "
    "this will return false.\n"
    ":Returns True if we can reject the null hypothesis (that the sketches reflect the same underlying "
    "distribution) using the provided p-value, otherwise False.");
  m.def("ks_test", &kolmogorov_smirnov::test<quantiles_sketch<double>>, nb::arg("sk_1"), nb::arg("sk_2"), nb::arg("p"),
    "Performs the Kolmogorov-Smirnov Test for :code:`quantiles_doubles_sketch` pairs.\n"
    "Note: if the given sketches have insufficient data or if the sketch sizes are too small, "
    "this will return false.\n"
    "Returns True if we can reject the null hypothesis (that the sketches reflect the same underlying "
    "distribution) using the provided p-value, otherwise False.");
  m.def("ks_test", &kolmogorov_smirnov::test<quantiles_sketch<nb::object, py_object_lt>>, nb::arg("sk_1"), nb::arg("sk_2"), nb::arg("p"),
    "Performs the Kolmogorov-Smirnov Test for :code:`quantiles_items_sketch` pairs.\n"
    "Note: if the given sketches have insufficient data or if the sketch sizes are too small, "
    "this will return false.\n"
    "Returns True if we can reject the null hypothesis (that the sketches reflect the same underlying "
    "distribution) using the provided p-value, otherwise False.");
}
