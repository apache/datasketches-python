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
#include <nanobind/stl/string.h>

#include "count_min.hpp"
#include "common_defs.hpp"

namespace nb = nanobind;

template<typename W>
void bind_count_min_sketch(nb::module_ &m, const char* name) {
  using namespace datasketches;

  nb::class_<count_min_sketch<W>>(m, name)
    .def(nb::init<uint8_t, uint32_t, uint64_t>(), nb::arg("num_hashes"), nb::arg("num_buckets"), nb::arg("seed")=DEFAULT_SEED,
         "Creates an instance of a CountMin sketch\n\n"
         ":param num_hashes: Number of rows in the sketch\n:type num_hashes: int\n"
         ":param num_buckets: Number of columns in the sketch\n:type num_buckets: int\n"
         ":param seed: Hash seed to use\n:type seed: int, optional"
         )
         // using nun_hashes (rows), num_buckets (columns), and hash seed `seed`.)
    .def("__copy__", [](const count_min_sketch<W>& sk){ return count_min_sketch<W>(sk); })
    .def_static("suggest_num_buckets", &count_min_sketch<W>::suggest_num_buckets, nb::arg("relative_error"),
                "Suggests the number of buckets needed to achieve an accuracy within the provided "
                "relative_error. For example, when relative_error = 0.05, the returned frequency estimates "
                "satisfy the 'relative_error' guarantee that never overestimates the weights but may "
                "underestimate the weights by 5% of the total weight in the sketch. "
                "Returns the number of hash buckets at every level of the sketch required in order to obtain "
                "the specified relative error.")
    .def_static("suggest_num_hashes", &count_min_sketch<W>::suggest_num_hashes, nb::arg("confidence"),
                "Suggests the number of hashes needed to achieve the provided confidence. For example, "
                "with 95% confidence, frequency estimates satisfy the 'relative_error' guarantee. "
                "Returns the number of hash functions that are required in order to achieve the specified "
                "confidence of the sketch. confidence = 1 - delta, with delta denoting the sketch failure probability.")
    .def("__str__", [](const count_min_sketch<W>& sk) { return sk.to_string(); },
         "Produces a string summary of the sketch")
    .def("to_string", &count_min_sketch<W>::to_string,
         "Produces a string summary of the sketch")
    .def("is_empty", &count_min_sketch<W>::is_empty,
         "Returns True if the sketch has seen no items, otherwise False")
    .def_prop_ro("num_hashes", &count_min_sketch<W>::get_num_hashes,
         "The configured number of hashes for the sketch")
    .def_prop_ro("num_buckets", &count_min_sketch<W>::get_num_buckets,
         "The configured number of buckets for the sketch")
    .def_prop_ro("seed", &count_min_sketch<W>::get_seed,
         "The base hash seed for the sketch")
    .def("get_relative_error", &count_min_sketch<W>::get_relative_error,
         "Returns the maximum permissible error for any frequency estimate query")
    .def_prop_ro("total_weight", &count_min_sketch<W>::get_total_weight,
         "The total weight currently inserted into the stream")
    .def("update", static_cast<void (count_min_sketch<W>::*)(int64_t, W)>(&count_min_sketch<W>::update), nb::arg("item"), nb::arg("weight")=1.0,
         "Updates the sketch with the given 64-bit integer value")
    .def("update", static_cast<void (count_min_sketch<W>::*)(const std::string&, W)>(&count_min_sketch<W>::update), nb::arg("item"), nb::arg("weight")=1.0,
         "Updates the sketch with the given string")
    .def("get_estimate", static_cast<W (count_min_sketch<W>::*)(int64_t) const>(&count_min_sketch<W>::get_estimate), nb::arg("item"),
         "Returns an estimate of the frequency of the provided 64-bit integer value")
    .def("get_estimate", static_cast<W (count_min_sketch<W>::*)(const std::string&) const>(&count_min_sketch<W>::get_estimate), nb::arg("item"),
         "Returns an estimate of the frequency of the provided string")
    .def("get_upper_bound", static_cast<W (count_min_sketch<W>::*)(int64_t) const>(&count_min_sketch<W>::get_upper_bound), nb::arg("item"),
         "Returns an upper bound on the estimate for the given 64-bit integer value")
    .def("get_upper_bound", static_cast<W (count_min_sketch<W>::*)(const std::string&) const>(&count_min_sketch<W>::get_upper_bound), nb::arg("item"),
         "Returns an upper bound on the estimate for the provided string")
    .def("get_lower_bound", static_cast<W (count_min_sketch<W>::*)(int64_t) const>(&count_min_sketch<W>::get_lower_bound), nb::arg("item"),
         "Returns a lower bound on the estimate for the given 64-bit integer value")
    .def("get_lower_bound", static_cast<W (count_min_sketch<W>::*)(const std::string&) const>(&count_min_sketch<W>::get_lower_bound), nb::arg("item"),
         "Returns a lower bound on the estimate for the provided string")
    .def("merge", &count_min_sketch<W>::merge, nb::arg("other"),
         "Merges the provided other sketch into this one")
    .def("get_serialized_size_bytes", &count_min_sketch<W>::get_serialized_size_bytes,
         "Returns the size in bytes of the serialized image of the sketch")
    .def(
        "serialize",
        [](const count_min_sketch<W>& sk) {
          auto bytes = sk.serialize();
          return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        },
        "Serializes the sketch into a bytes object"
    )
    .def_static(
        "deserialize",
        [](const nb::bytes& bytes) { return count_min_sketch<W>::deserialize(bytes.c_str(), bytes.size()); },
        nb::arg("bytes"),
        "Reads a bytes object and returns the corresponding count_min_sketch"
    );
}

void init_count_min(nb::module_ &m) {
  bind_count_min_sketch<double>(m, "count_min_sketch");
}

