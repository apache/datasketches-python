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

#include "cpc_sketch.hpp"
#include "cpc_union.hpp"
#include "cpc_common.hpp"
#include "common_defs.hpp"

namespace nb = nanobind;

void init_cpc(nb::module_ &m) {
  using namespace datasketches;

  nb::class_<cpc_sketch>(m, "cpc_sketch")
    .def(nb::init<uint8_t, uint64_t>(), nb::arg("lg_k")=cpc_constants::DEFAULT_LG_K, nb::arg("seed")=DEFAULT_SEED,
         "Creates a new CPC sketch\n\n"
         ":param lg_k: base 2 logarithm of the number of bins in the sketch\n"
         ":type lg_k: int, optional\n"
         ":param seed: seed value for the hash function\n"
         ":type seed: int, optional"
    )
    .def("__copy__", [](const cpc_sketch& sk){ return cpc_sketch(sk); })
    .def("__str__", [](const cpc_sketch& sk) { return sk.to_string(); },
         "Produces a string summary of the sketch")
    .def("to_string", &cpc_sketch::to_string,
         "Produces a string summary of the sketch")
    .def<void (cpc_sketch::*)(uint64_t)>("update", &cpc_sketch::update, nb::arg("datum"),
         "Updates the sketch with the given 64-bit integer value")
    .def<void (cpc_sketch::*)(double)>("update", &cpc_sketch::update, nb::arg("datum"),
         "Updates the sketch with the given 64-bit floating point")
    .def<void (cpc_sketch::*)(const std::string&)>("update", &cpc_sketch::update, nb::arg("datum"),
         "Updates the sketch with the given string")
    .def_prop_ro("lg_k", &cpc_sketch::get_lg_k,
         "Configured lg_k of this sketch")
    .def("is_empty", &cpc_sketch::is_empty,
         "Returns True if the sketch is empty, otherwise False")
    .def("get_estimate", &cpc_sketch::get_estimate,
         "Estimate of the distinct count of the input stream")
    .def("get_lower_bound", &cpc_sketch::get_lower_bound, nb::arg("kappa"),
         "Returns an approximate lower bound on the estimate for kappa values in {1, 2, 3}, roughly corresponding to standard deviations")
    .def("get_upper_bound", &cpc_sketch::get_upper_bound, nb::arg("kappa"),
         "Returns an approximate upper bound on the estimate for kappa values in {1, 2, 3}, roughly corresponding to standard deviations")
    .def(
        "serialize",
        [](const cpc_sketch& sk) {
          auto bytes = sk.serialize();
          return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        },
        "Serializes the sketch into a bytes object"
    )
    .def_static(
        "deserialize",
        [](const nb::bytes& bytes) { return cpc_sketch::deserialize(bytes.c_str(), bytes.size()); },
        nb::arg("bytes"),
        "Reads a bytes object and returns the corresponding cpc_sketch"
    );

  nb::class_<cpc_union>(m, "cpc_union")
    .def(nb::init<uint8_t, uint64_t>(), nb::arg("lg_k"), nb::arg("seed")=DEFAULT_SEED)
    .def("update", (void (cpc_union::*)(const cpc_sketch&)) &cpc_union::update, nb::arg("sketch"),
         "Updates the union with the provided CPC sketch")
    .def("get_result", &cpc_union::get_result,
         "Returns a CPC sketch with the result of the union")
    ;
}
