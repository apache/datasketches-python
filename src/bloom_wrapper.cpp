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

#include <algorithm> // TODO: remove when bloom_filter_impl.hpp includes this

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>

#include "bloom_filter.hpp"
#include "common_defs.hpp"

namespace nb = nanobind;

void init_bloom(nb::module_ &m) {
  using namespace datasketches;

  nb::class_<bloom_filter>(m, "bloom_filter")
    .def_static("suggest_num_hashes",
        nb::overload_cast<uint64_t, uint64_t>(&bloom_filter::builder::suggest_num_hashes),
        nb::arg("max_distinct_items"),
        nb::arg("num_filter_bits")
    )
    .def_static("suggest_num_hashes",
        nb::overload_cast<double>(&bloom_filter::builder::suggest_num_hashes),
        nb::arg("target_false_positive_prob")
    )
    .def_static("suggest_num_filter_bits",
        nb::overload_cast<uint64_t, double>(&bloom_filter::builder::suggest_num_filter_bits),
        nb::arg("max_distinct_items"),
        nb::arg("target_false_positive_prob")
    )
    .def_static("create_by_accuracy",
        [](uint64_t max_distinct_items, double target_fpp, std::optional<uint64_t> seed) {
          return bloom_filter::builder::create_by_accuracy(max_distinct_items,
            target_fpp,
            seed.value_or(bloom_filter::builder::generate_random_seed()));
        },
        nb::arg("max_distinct_items"),
        nb::arg("target_false_positive_prob"),
        nb::arg("seed") = nb::none()
    )
    .def_static("create_by_size",
        [](uint64_t num_bits, uint16_t num_hashes, std::optional<uint64_t> seed) {
          return bloom_filter::builder::create_by_size(num_bits,
            num_hashes,
            seed.value_or(bloom_filter::builder::generate_random_seed()));
        },
        nb::arg("num_bits"),
        nb::arg("num_hashes"),
        nb::arg("seed") = nb::none()
    )


    .def_static("deserialize",
        [](const nb::bytes& bytes) { return bloom_filter::deserialize(bytes.c_str(), bytes.size()); },
        nb::arg("bytes"),
        "Reads a bytes object and returns the corresponding bloom_filter"
    )
    .def_static(
        "wrap",
        [](const nb::bytes& bytes) { return bloom_filter::wrap(bytes.c_str(), bytes.size()); },
        nb::arg("bytes"),
        "Wraps the provided bytearray as a read-only Bloom filter"
    )
    .def_static(
        "writable_wrap",
        [](const nb::bytearray& bytearray) {
          return bloom_filter::writable_wrap(const_cast<char*>(bytearray.c_str()), bytearray.size());
        },
        nb::arg("bytearray"),
        "Wraps the provided bytearray as a writable Bloom filter"
    )
    .def("serialize",
        [](const bloom_filter& bf) {
          auto bytes = bf.serialize();
          return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        },
        "Serializes the filter into a bytes object"
    )

    .def("__copy__", [](const bloom_filter& bf){ return bloom_filter(bf); })
    .def("__str__", [](const bloom_filter& bf) { return bf.to_string(false); },
         "Produces a string summary of the filter")
    .def("to_string", &bloom_filter::to_string,
         nb::arg("print_filter") = false,
         "Produces a string summary of the filter")

    .def("update", nb::overload_cast<uint64_t>(&bloom_filter::update), nb::arg("item"),
         "Updates the sketch with the given 64-bit integer value")
    .def("update", nb::overload_cast<double>(&bloom_filter::update), nb::arg("item"),
         "Updates the sketch with the given 64-bit floating point value")
    .def("update", nb::overload_cast<const std::string&>(&bloom_filter::update), nb::arg("item"),
         "Updates the sketch with the given string")

    .def("query_and_update", nb::overload_cast<uint64_t>(&bloom_filter::query_and_update), nb::arg("item"),
         "Updates the sketch with the given 64-bit integer value\n"
         "and returns the value from querying prior to the update")
    .def("query_and_update", nb::overload_cast<double>(&bloom_filter::query_and_update), nb::arg("item"),
         "Updates the sketch with the given 64-bit floating point value\n"
         "and returns the value from querying prior to the update")
    .def("query_and_update", nb::overload_cast<const std::string&>(&bloom_filter::query_and_update), nb::arg("item"),
         "Updates the sketch with the given string and returns\n"
         "the value from querying prior to the update")

    .def("query", nb::overload_cast<uint64_t>(&bloom_filter::query, nb::const_), nb::arg("item"),
         "Queries the sketch with the given 64-bit integer value and\n"
         "returns whether the value might have been seen previously")
    .def("query", nb::overload_cast<double>(&bloom_filter::query, nb::const_), nb::arg("item"),
         "Updates the sketch with the given 64-bit floating point value and\n"
         "returns whether the value might have been seen previously")
    .def("query", nb::overload_cast<const std::string&>(&bloom_filter::query, nb::const_), nb::arg("item"),
         "Updates the sketch with the given string and returns whether the\n"
         "string might have been seen previously")

    .def("union", &bloom_filter::union_with, nb::arg("other"),
         "Unions two Bloom filters by applying a logical OR. The result will\n"
         "recognize any values seen by either filter, as well as false positives")
    .def("intersect", &bloom_filter::intersect, nb::arg("other"),
         "Intersects two Bloom filters by applying a logical AND. The result will\n"
         "recognize only values seen by both filter, as well as false positives")
    .def("invert", &bloom_filter::invert,
         "Inverts all the bits of the filter. Approximately inverts the notion of set-membership.")

    .def("is_empty", &bloom_filter::is_empty,
         "Returns True if the filter is empty, otherwise False")
    .def("get_bits_used", &bloom_filter::get_bits_used,
         "Returns the number of bits in the Bloom filter that are set to 1")
    .def("get_capacity", &bloom_filter::get_capacity,
         "Returns the total number of bits in the Bloom filter")
    .def("get_num_hashes", &bloom_filter::get_num_hashes,
         "Returns the configured number of hash functions for this Bloom filter")
    .def("get_seed", &bloom_filter::get_seed,
         "Returns the hash seed for this Bloom filter")
    .def("reset", &bloom_filter::reset,
         "Returns the Bloom filter to its original empty state")

    .def("get_serialized_size_bytes", [](const bloom_filter& bf) { return bf.get_serialized_size_bytes(); },
         "Returns the number of bytes needed to serialize the Bloom filter")
    .def_static("get_serialized_size_bytes_given_bits", [](uint64_t num_bits) { return bloom_filter::get_serialized_size_bytes(num_bits); },
        nb::arg("num_bits"),
         "Returns the number of bytes needed to serialize a Bloom filter with a capacity of num_bits")
  ;

}
