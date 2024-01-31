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

#include "hll.hpp"

namespace nb = nanobind;

void init_hll(nb::module_ &m) {
  using namespace datasketches;

  nb::enum_<target_hll_type>(m, "tgt_hll_type", "Target HLL flavor")
    .value("HLL_4", HLL_4)
    .value("HLL_6", HLL_6)
    .value("HLL_8", HLL_8)
    .export_values();

  nb::class_<hll_sketch>(m, "hll_sketch")
    .def(nb::init<uint8_t, target_hll_type, bool>(), nb::arg("lg_k"), nb::arg("tgt_type")=HLL_8, nb::arg("start_max_size")=false,
         "Constructs a new HLL sketch\n\n"
         ":param lg_config_k: A full sketch can hold 2^lg_config_k rows. Must be between 7 and 21, inclusive,\n"
         ":type lg_config_k: int\n"
         ":param tgt_type: The HLL mode to use, if/when the sketch reaches estimation mode\n"
         ":type tgt_type: tgt_hll_type\n"
         ":param start_full_size: Indicates whether to start in HLL mode, keeping memory use constant (if HLL_6 or "
         "HLL_8) at the cost of much higher initial memory use. Default (and recommended) is False.\n"
         ":type start_full_size: bool"
     )
    .def("__str__", [](const hll_sketch& sk) { return sk.to_string(); },
         "Produces a string summary of the sketch")
    .def("to_string", (std::string (hll_sketch::*)(bool,bool,bool,bool) const) &hll_sketch::to_string,
         nb::arg("summary")=true, nb::arg("detail")=false, nb::arg("aux_detail")=false, nb::arg("all")=false,
         "Produces a string summary of the sketch")
    .def_prop_ro("lg_config_k", &hll_sketch::get_lg_config_k, "Configured lg_k value for the sketch")
    .def_prop_ro("tgt_type", &hll_sketch::get_target_type, "The HLL type (4, 6, or 8) when in estimation mode")
    .def("get_estimate", &hll_sketch::get_estimate,
         "Estimate of the distinct count of the input stream")
    .def("get_lower_bound", &hll_sketch::get_lower_bound, nb::arg("num_std_devs"),
         "Returns the approximate lower error bound given the specified number of standard deviations in {1, 2, 3}")
    .def("get_upper_bound", &hll_sketch::get_upper_bound, nb::arg("num_std_devs"),
         "Returns the approximate upper error bound given the specified number of standard deviations in {1, 2, 3}")
    .def("is_compact", &hll_sketch::is_compact,
         "True if the sketch is compact, otherwise False")
    .def("is_empty", &hll_sketch::is_empty,
         "True if the sketch is empty, otherwise False")
    .def("get_updatable_serialization_bytes", &hll_sketch::get_updatable_serialization_bytes,
         "Returns the size of the serialized sketch")
    .def("get_compact_serialization_bytes", &hll_sketch::get_compact_serialization_bytes,
         "Returns the size of the serialized sketch when compressing the exception table if HLL_4")
    .def("reset", &hll_sketch::reset,
         "Resets the sketch to the empty state in coupon collection mode")
    .def("update", (void (hll_sketch::*)(int64_t)) &hll_sketch::update, nb::arg("datum"),
         "Updates the sketch with the given integral value")
    .def("update", (void (hll_sketch::*)(double)) &hll_sketch::update, nb::arg("datum"),
         "Updates the sketch with the given floating point value")
    .def("update", (void (hll_sketch::*)(const std::string&)) &hll_sketch::update, nb::arg("datum"),
         "Updates the sketch with the given string value")
    .def_static("get_max_updatable_serialization_bytes", &hll_sketch::get_max_updatable_serialization_bytes,
         nb::arg("lg_k"), nb::arg("tgt_type"),
         "Provides a likely upper bound on serialization size for the given parameters")
    .def_static("get_rel_err", &hll_sketch::get_rel_err,
         nb::arg("upper_bound"), nb::arg("unioned"), nb::arg("lg_k"), nb::arg("num_std_devs"),
         "Returns the a priori relative error bound for the given parameters")
    .def(
        "serialize_compact",
        [](const hll_sketch& sk) {
          auto bytes = sk.serialize_compact();
          return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        },
        "Serializes the sketch into a bytes object, compressing the exception table if HLL_4"
    )
    .def(
        "serialize_updatable",
        [](const hll_sketch& sk) {
          auto bytes = sk.serialize_updatable();
          return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        },
        "Serializes the sketch into a bytes object"
    )
    .def_static(
        "deserialize",
        [](const nb::bytes& bytes) { return hll_sketch::deserialize(bytes.c_str(), bytes.size()); },
        nb::arg("bytes"),
        "Reads a bytes object and returns the corresponding hll_sketch"
    );

  nb::class_<hll_union>(m, "hll_union")
    .def(nb::init<uint8_t>(), nb::arg("lg_max_k"),
         "Construct an hll_union object if the given size.\n\n"
         ":param lg_max_k: The maximum size, in log2, of k. Must be between 7 and 21, inclusive.\n"
         ":type lg_max_k: int"
         )
    .def_prop_ro("lg_config_k", &hll_union::get_lg_config_k, "Configured lg_k value for the union")
    .def("get_estimate", &hll_union::get_estimate,
         "Estimate of the distinct count of the input stream")
    .def("get_lower_bound", &hll_union::get_lower_bound, nb::arg("num_std_devs"),
         "Returns the approximate lower error bound given the specified number of standard deviations in {1, 2, 3}")
    .def("get_upper_bound", &hll_union::get_upper_bound, nb::arg("num_std_devs"),
         "Returns the approximate upper error bound given the specified number of standard deviations in {1, 2, 3}")
    .def("is_empty", &hll_union::is_empty,
         "True if the union is empty, otherwise False")    
    .def("reset", &hll_union::reset,
         "Resets the union to the empty state")
    .def("get_result", &hll_union::get_result, nb::arg("tgt_type")=HLL_4,
         "Returns a sketch of the target type representing the current union state")
    .def<void (hll_union::*)(const hll_sketch&)>("update", &hll_union::update, nb::arg("sketch"),
         "Updates the union with the given HLL sketch")
    .def<void (hll_union::*)(int64_t)>("update", &hll_union::update, nb::arg("datum"),
         "Updates the union with the given integral value")
    .def<void (hll_union::*)(double)>("update", &hll_union::update, nb::arg("datum"),
         "Updates the union with the given floating point value")
    .def<void (hll_union::*)(const std::string&)>("update", &hll_union::update, nb::arg("datum"),
         "Updates the union with the given string value")
    .def_static("get_rel_err", &hll_union::get_rel_err,
         nb::arg("upper_bound"), nb::arg("unioned"), nb::arg("lg_k"), nb::arg("num_std_devs"),
         "Returns the a priori relative error bound for the given parameters")
    ;
}
