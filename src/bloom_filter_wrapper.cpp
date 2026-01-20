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

#include "bloom_filter.hpp"
#include "common_defs.hpp"

namespace nb = nanobind;

template<typename A>
void bind_bloom_filter(nb::module_ &m, const char* name) {
  using namespace datasketches;
  using bloom_filter_type = bloom_filter_alloc<A>;

  // Bind the class with static factory methods only
  nb::class_<bloom_filter_type>(m, name)
    .def("is_empty", &bloom_filter_type::is_empty,
         "Returns True if the filter has seen no items, otherwise False")
    
    // Update methods - efficient overloads for Python types
    // Non-negative integers (uint64_t)
    .def("update", static_cast<void (bloom_filter_type::*)(uint64_t)>(&bloom_filter_type::update), 
         nb::arg("item"),
         "Updates the filter with a non-negative integer")
    // Negative/positive integers (int64_t)
    .def("update", static_cast<void (bloom_filter_type::*)(int64_t)>(&bloom_filter_type::update), 
         nb::arg("item"),
         "Updates the filter with a negative/positive integer")
    // Float (double)
    .def("update", static_cast<void (bloom_filter_type::*)(double)>(&bloom_filter_type::update), 
         nb::arg("item"),
         "Updates the filter with a float")
    // String (std::string)
    .def("update", static_cast<void (bloom_filter_type::*)(const std::string&)>(&bloom_filter_type::update), 
         nb::arg("item"),
         "Updates the filter with a string")
    // Bytes object
    .def("update",
         [](bloom_filter_type& self, nb::bytes b) {
           self.update(b.c_str(), b.size());
         },
         nb::arg("item"),
         "Updates the filter with a bytes object")
    
    // Query methods - efficient overloads for Python types
    // Non-negative integers (uint64_t)
    .def("query", static_cast<bool (bloom_filter_type::*)(uint64_t) const>(&bloom_filter_type::query), 
         nb::arg("item"),
         "Queries the filter for a non-negative integer")
    // Negative/positive integers (int64_t)
    .def("query", static_cast<bool (bloom_filter_type::*)(int64_t) const>(&bloom_filter_type::query), 
         nb::arg("item"),
         "Queries the filter for a negative/positive integer")
    // Float (double)
    .def("query", static_cast<bool (bloom_filter_type::*)(double) const>(&bloom_filter_type::query), 
         nb::arg("item"),
         "Queries the filter for a float")
    // String (std::string)
    .def("query", static_cast<bool (bloom_filter_type::*)(const std::string&) const>(&bloom_filter_type::query), 
         nb::arg("item"),
         "Queries the filter for a string")
    // Bytes object
    .def("query",
         [](const bloom_filter_type& self, nb::bytes b) -> bool {
           return self.query(b.c_str(), b.size());
         },
         nb::arg("item"),
         "Queries the filter for a bytes object")
    
    .def("reset", &bloom_filter_type::reset,
         "Resets the Bloom filter to its original empty state")
    .def("union_with", &bloom_filter_type::union_with,
         nb::arg("other"),
         "Performs a union operation with another Bloom filter. Both filters must have the same capacity, number of hashes, and seed.")
    .def("intersect", &bloom_filter_type::intersect,
         nb::arg("other"),
         "Performs an intersection operation with another Bloom filter. Both filters must have the same capacity, number of hashes, and seed.")
    .def("invert", &bloom_filter_type::invert,
         "Inverts all the bits of the BloomFilter. Approximately inverts the notion of set-membership.")
    .def("to_string", &bloom_filter_type::to_string,
         nb::arg("print_filter")=false,
         "Returns a string representation of the Bloom filter\n\n"
         ":param print_filter: If True, includes the actual bit array in the output\n:type print_filter: bool, optional\n"
         ":return: String representation of the filter\n:rtype: str")
    .def("__str__", [](const bloom_filter_type& self) { return self.to_string(false); },
         "Returns a string summary of the Bloom filter (without printing the bit array)")
    .def("__repr__", [](const bloom_filter_type& self) { return self.to_string(false); },
         "Returns a detailed string representation of the Bloom filter for debugging and REPL use")
    .def("__copy__", [](const bloom_filter_type& self) { return bloom_filter_type(self); },
         "Returns a copy of the Bloom filter")
    .def("is_compatible", &bloom_filter_type::is_compatible,
         nb::arg("other"),
         "Returns True if the other Bloom filter is compatible for union/intersection operations (same capacity, num_hashes, and seed)")
    .def("get_serialized_size_bytes", 
         [](const bloom_filter_type& sk) { return sk.get_serialized_size_bytes(); },
         "Returns the size in bytes of the serialized image of the filter")
    .def("serialize",
        [](const bloom_filter_type& sk) {
            auto v = sk.serialize(); // vector_bytes (std::vector<uint8_t, Allocator>)
            return nb::bytes(reinterpret_cast<const char*>(v.data()), v.size());
        },
        "Serialize the filter to a cross-language compatible byte string")
    .def_static(
        "deserialize",
        [](const nb::bytes& bytes) {
            return bloom_filter_type::deserialize(bytes.c_str(), bytes.size());
        },
        nb::arg("bytes"),
        "Reads a bytes object and returns the corresponding bloom_filter")
    .def_static("suggest_num_hashes", 
                static_cast<uint16_t (*)(uint64_t, uint64_t)>(&bloom_filter_type::builder::suggest_num_hashes),
                nb::arg("max_distinct_items"), nb::arg("num_filter_bits"),
                "Suggests the optimal number of hash functions for given target numbers of distinct items and filter size")
    .def_static("suggest_num_hashes_by_probability", 
                static_cast<uint16_t (*)(double)>(&bloom_filter_type::builder::suggest_num_hashes),
                nb::arg("target_false_positive_prob"),
                "Suggests the optimal number of hash functions to achieve a target false positive probability")
    .def_static("suggest_num_filter_bits", 
                &bloom_filter_type::builder::suggest_num_filter_bits,
                nb::arg("max_distinct_items"), nb::arg("target_false_positive_prob"),
                "Suggests the optimal number of bits for given target numbers of distinct items and false positive probability")
    .def_static("create_by_accuracy",
                [](uint64_t max_distinct_items, double target_false_positive_prob, uint64_t seed) {
                  return bloom_filter_type::builder::create_by_accuracy(max_distinct_items, target_false_positive_prob, seed);
                },
                nb::arg("max_distinct_items"), nb::arg("target_false_positive_prob"), nb::arg("seed")=bloom_filter_type::builder::generate_random_seed(),
                "Creates a Bloom filter with optimal parameters for the given accuracy requirements\n\n"
                ":param max_distinct_items: Maximum expected number of distinct items to add to the filter\n:type max_distinct_items: int\n"
                ":param target_false_positive_prob: Desired false positive probability per item\n:type target_false_positive_prob: float\n"
                ":param seed: Hash seed to use (default: random)\n:type seed: int, optional"
                )
    .def_static("create_by_size",
                [](uint64_t num_bits, uint16_t num_hashes, uint64_t seed) {
                  return bloom_filter_type::builder::create_by_size(num_bits, num_hashes, seed);
                },
                nb::arg("num_bits"), nb::arg("num_hashes"), nb::arg("seed")=bloom_filter_type::builder::generate_random_seed(),
                "Creates a Bloom filter with specified size parameters\n\n"
                ":param num_bits: Size of the Bloom filter in bits\n:type num_bits: int\n"
                ":param num_hashes: Number of hash functions to apply to items\n:type num_hashes: int\n"
                ":param seed: Hash seed to use (default: random)\n:type seed: int, optional"
                )
    .def_prop_ro(
        "num_bits_used",
        &bloom_filter_type::get_bits_used,
        "Number of bits set to 1 in the Bloom filter"
        )
    .def_prop_ro(
        "capacity",
        &bloom_filter_type::get_capacity,
        "Number of bits in the Bloom filter's bit array"
        )
    .def_prop_ro(
        "num_hashes",
        &bloom_filter_type::get_num_hashes,
        "Number of hash functions used by this Bloom filter"
        )
    .def_prop_ro(
        "seed",
        &bloom_filter_type::get_seed,
        "Hash seed used by this Bloom filter"
        );
}

void init_bloom_filter(nb::module_ &m) {
  bind_bloom_filter<std::allocator<uint8_t>>(m, "bloom_filter");
} 