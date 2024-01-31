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

#include <nanobind/nanobind.h>
#include <nanobind/make_iterator.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "py_object_lt.hpp"
#include "py_object_ostream.hpp"
#include "quantile_conditional.hpp"
#include "quantiles_sketch.hpp"


namespace nb = nanobind;

template<typename T, typename C>
void bind_quantiles_sketch(nb::module_ &m, const char* name) {
  using namespace datasketches;

  auto quantiles_class = nb::class_<quantiles_sketch<T, C>>(m, name)
    .def(nb::init<uint16_t>(), nb::arg("k")=quantiles_constants::DEFAULT_K,
         "Creates a classic quantiles sketch instance with the given value of k.\n\n"
         ":param k: Controls the size/accuracy trade-off of the sketch. Default is 128.\n"
         ":type k: int, optional"
    )
    .def("__copy__", [](const quantiles_sketch<T, C>& sk) { return quantiles_sketch<T,C>(sk); })
    .def(
        "update",
        static_cast<void (quantiles_sketch<T, C>::*)(const T&)>(&quantiles_sketch<T, C>::update),
        nb::arg("item"),
        "Updates the sketch with the given value"
    )
    .def("merge", (void (quantiles_sketch<T, C>::*)(const quantiles_sketch<T, C>&)) &quantiles_sketch<T, C>::merge, nb::arg("sketch"),
         "Merges the provided sketch into this one")
    .def("__str__", [](const quantiles_sketch<T, C>& sk) { return sk.to_string(); },
         "Produces a string summary of the sketch")
    .def("to_string", &quantiles_sketch<T, C>::to_string, nb::arg("print_levels")=false, nb::arg("print_items")=false,
         "Produces a string summary of the sketch")
    .def("is_empty", &quantiles_sketch<T, C>::is_empty,
         "Returns True if the sketch is empty, otherwise False")
    .def_prop_ro("k", &quantiles_sketch<T, C>::get_k,
         "The configured parameter k")
    .def_prop_ro("n", &quantiles_sketch<T, C>::get_n,
         "The length of the input stream")
    .def_prop_ro("num_retained", &quantiles_sketch<T, C>::get_num_retained,
         "The number of retained items (samples) in the sketch")
    .def("is_estimation_mode", &quantiles_sketch<T, C>::is_estimation_mode,
         "Returns True if the sketch is in estimation mode, otherwise False")
    .def("get_min_value", &quantiles_sketch<T, C>::get_min_item,
         "Returns the minimum value from the stream. If empty, quantiles_floats_sketch returns nan; quantiles_ints_sketch throws a RuntimeError")
    .def("get_max_value", &quantiles_sketch<T, C>::get_max_item,
         "Returns the maximum value from the stream. If empty, quantiles_floats_sketch returns nan; quantiles_ints_sketch throws a RuntimeError")
    .def("get_quantile", &quantiles_sketch<T, C>::get_quantile, nb::arg("rank"), nb::arg("inclusive")=false,
         "Returns an approximation to the data value "
         "associated with the given rank in a hypothetical sorted "
         "version of the input stream so far.\n"
         "For quantiles_floats_sketch: if the sketch is empty this returns nan. "
         "For quantiles_ints_sketch: if the sketch is empty this throws a RuntimeError.")
    .def(
        "get_quantiles",
        [](const quantiles_sketch<T, C>& sk, const std::vector<double>& ranks, bool inclusive) {
          std::vector<T> quantiles;
          if (!sk.is_empty()) {
            quantiles.reserve(ranks.size());
            for (size_t i = 0; i < ranks.size(); ++i) quantiles.push_back(sk.get_quantile(ranks[i], inclusive));
          }
          return quantiles;
        },
        nb::arg("ranks"), nb::arg("inclusive")=false,
        "This returns an array that could have been generated by using get_quantile() for each "
        "normalized rank separately.\n"
        "If the sketch is empty this returns an empty vector."
    )
    .def("get_rank", &quantiles_sketch<T, C>::get_rank, nb::arg("value"), nb::arg("inclusive")=false,
         "Returns an approximation to the normalized rank of the given value from 0 to 1, inclusive.\n"
         "The resulting approximation has a probabilistic guarantee that can be obtained from the "
         "get_normalized_rank_error(False) function.\n"
         "With the parameter inclusive=true the weight of the given value is included into the rank."
         "Otherwise the rank equals the sum of the weights of values less than the given value.\n"
         "If the sketch is empty this returns nan.")
    .def(
        "get_pmf",
        [](const quantiles_sketch<T, C>& sk, const std::vector<T>& split_points, bool inclusive) {
          return sk.get_PMF(split_points.data(), split_points.size(), inclusive);
        },
        nb::arg("split_points"), nb::arg("inclusive")=false,
        "Returns an approximation to the Probability Mass Function (PMF) of the input stream "
        "given a set of split points (values).\n"
        "The resulting approximations have a probabilistic guarantee that can be obtained from the "
        "get_normalized_rank_error(True) function.\n"
        "If the sketch is empty this returns an empty vector.\n"
        "split_points is an array of m unique, monotonically increasing float values "
        "that divide the real number line into m+1 consecutive disjoint intervals.\n"
        "The definition of an 'interval' is inclusive of the left split point (or minimum value) and "
        "exclusive of the right split point, with the exception that the last interval will include "
        "the maximum value.\n"
        "It is not necessary to include either the min or max values in these split points."
    )
    .def(
        "get_cdf",
        [](const quantiles_sketch<T, C>& sk, const std::vector<T>& split_points, bool inclusive) {
          return sk.get_CDF(split_points.data(), split_points.size(), inclusive);
        },
        nb::arg("split_points"), nb::arg("inclusive")=false,
        "Returns an approximation to the Cumulative Distribution Function (CDF), which is the "
        "cumulative analog of the PMF, of the input stream given a set of split points (values).\n"
        "The resulting approximations have a probabilistic guarantee that can be obtained from the "
        "get_normalized_rank_error(True) function.\n"
        "If the sketch is empty this returns an empty vector.\n"
        "split_points is an array of m unique, monotonically increasing float values "
        "that divide the real number line into m+1 consecutive disjoint intervals.\n"
        "The definition of an 'interval' is inclusive of the left split point (or minimum value) and "
        "exclusive of the right split point, with the exception that the last interval will include "
        "the maximum value.\n"
        "It is not necessary to include either the min or max values in these split points."
    )
    .def(
        "normalized_rank_error",
        static_cast<double (quantiles_sketch<T, C>::*)(bool) const>(&quantiles_sketch<T, C>::get_normalized_rank_error),
        nb::arg("as_pmf"),
        "Gets the normalized rank error for this sketch.\n"
        "If pmf is True, returns the 'double-sided' normalized rank error for the get_PMF() function.\n"
        "Otherwise, it is the 'single-sided' normalized rank error for all the other queries.\n"
        "Constants were derived as the best fit to 99 percentile empirically measured max error in thousands of trials"
    )
    .def_static(
        "get_normalized_rank_error",
        [](uint16_t k, bool pmf) { return quantiles_sketch<T, C>::get_normalized_rank_error(k, pmf); },
        nb::arg("k"), nb::arg("as_pmf"),
        "Gets the normalized rank error given parameters k and the pmf flag.\n"
        "If pmf is True, returns the 'double-sided' normalized rank error for the get_PMF() function.\n"
        "Otherwise, it is the 'single-sided' normalized rank error for all the other queries.\n"
        "Constants were derived as the best fit to 99 percentile empirically measured max error in thousands of trials"
    )
    .def("__iter__",
          [](const quantiles_sketch<T, C>& s) {
               return nb::make_iterator(nb::type<quantiles_sketch<T, C>>(),
               "quantiles_iterator",
               s.begin(),
               s.end());
          }, nb::keep_alive<0,1>()
     )
     ;

    add_serialization<T>(quantiles_class);
    add_vector_update<T>(quantiles_class);
}

void init_quantiles(nb::module_ &m) {
  bind_quantiles_sketch<int, std::less<int>>(m, "quantiles_ints_sketch");
  bind_quantiles_sketch<float, std::less<float>>(m, "quantiles_floats_sketch");
  bind_quantiles_sketch<double, std::less<double>>(m, "quantiles_doubles_sketch");
  bind_quantiles_sketch<nb::object, py_object_lt>(m, "quantiles_items_sketch");
}
