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

#include <vector>
#include <stdexcept>
#include <algorithm> // should ultimately be in tdigest.hpp

#include <nanobind/nanobind.h>
#include <nanobind/make_iterator.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/ndarray.h>

#include "tdigest.hpp"
#include "quantile_conditional.hpp"

namespace nb = nanobind;

template<typename T>
void bind_tdigest(nb::module_ &m, const char* name) {
  using namespace datasketches;

  auto tdigest_class = nb::class_<tdigest<T>>(m, name)
    .def(nb::init<uint16_t>(), nb::arg("k")=tdigest<T>::DEFAULT_K,
         "Creates a tdigest instance with the given value of k.\n\n"
         ":param k: Controls the size/accuracy trade-off of the sketch. Default is 200.\n"
         ":type k: int, optional"
    )
    .def("__copy__", [](const tdigest<T>& sk) { return tdigest<T>(sk); })
    .def("update", (void(tdigest<T>::*)(T)) &tdigest<T>::update, nb::arg("item"),
        "Updates the sketch with the given value")
    .def("merge", (void(tdigest<T>::*)(const tdigest<T>&)) &tdigest<T>::merge, nb::arg("sketch"),
         "Merges the provided sketch into this one")
    .def("__str__", [](const tdigest<T>& sk) { return sk.to_string(); },
         "Produces a string summary of the sketch")
    .def("to_string", &tdigest<T>::to_string, nb::arg("print_centroids")=false,
         "Produces a string summary of the sketch")
    .def("is_empty", &tdigest<T>::is_empty,
         "Returns True if the sketch is empty, otherwise False")
    .def_prop_ro("k", &tdigest<T>::get_k,
         "The configured parameter k")
    .def("get_total_weight", &tdigest<T>::get_total_weight,
         "The total weight processed by the sketch")
    .def("compress", &tdigest<T>::compress,
         "Process buffered values and merge centroids, if necesssary")
    .def("get_min_value", &tdigest<T>::get_min_value,
         "Returns the minimum value from the stream. If empty, throws a RuntimeError")
    .def("get_max_value", &tdigest<T>::get_max_value,
         "Returns the maximum value from the stream. If empty, throws a RuntimeError")
    .def("get_rank", &tdigest<T>::get_rank, nb::arg("value"),
         "Computes the approximate normalized rank of the given value")
    .def("get_quantile", &tdigest<T>::get_quantile, nb::arg("rank"),
         "Returns an approximation to the data value "
         "associated with the given rank in a hypothetical sorted "
         "version of the input stream so far.\n")
    .def("get_serialized_size_bytes", &tdigest<T>::get_serialized_size_bytes,
         nb::arg("with_buffer")=false,
         "Returns the size of the serialized sketch, in bytes")
    .def(
        "get_pmf",
        [](const tdigest<T>& sk, const std::vector<T>& split_points) {
          return sk.get_PMF(split_points.data(), split_points.size());
        },
        nb::arg("split_points"),
        "Returns an approximation to the Probability Mass Function (PMF) of the input stream "
        "given a set of split points (values).\n"
        "If the sketch is empty this returns an empty vector.\n"
        "split_points is an array of m unique, monotonically increasing float values "
        "that divide the real number line into m+1 consecutive disjoint intervals.\n"
        "It is not necessary to include either the min or max values in these split points."
    )
    .def(
        "get_cdf",
        [](const tdigest<T>& sk, const std::vector<T>& split_points) {
          return sk.get_CDF(split_points.data(), split_points.size());
        },
        nb::arg("split_points"),
        "Returns an approximation to the Cumulative Distribution Function (CDF), which is the "
        "cumulative analog of the PMF, of the input stream given a set of split points (values).\n"
        "If the sketch is empty this returns an empty vector.\n"
        "split_points is an array of m unique, monotonically increasing float values "
        "that divide the real number line into m+1 consecutive disjoint intervals.\n"
        "It is not necessary to include either the min or max values in these split points."
    )
    ;

    add_serialization<T>(tdigest_class);
    add_vector_update<T>(tdigest_class);
}

void init_tdigest(nb::module_ &m) {
  bind_tdigest<float>(m, "tdigest_float");
  bind_tdigest<double>(m, "tdigest_double");
}
