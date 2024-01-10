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

#ifndef _QUANTILE_CONDITIONAL_HPP_
#define _QUANTILE_CONDITIONAL_HPP_

/*
  This header defines conditionally compiled functions shared
  across the set of quantile family sketches.
*/

#include "common_defs.hpp"
#include "py_serde.hpp"

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/string.h>
#include <nanobind/ndarray.h>

namespace nb = nanobind;

// Serialization
// std::string and arithmetic types, where we don't need a separate serde
template<typename T, typename SK, typename std::enable_if<std::is_arithmetic<T>::value || std::is_same<std::string, T>::value, bool>::type = 0>
void add_serialization(nb::class_<SK>& clazz) {
  clazz.def(
        "serialize",
        [](const SK& sk) {
          auto bytes = sk.serialize();
          return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        },
        "Serializes the sketch into a bytes object."
    )
    .def_static(
        "deserialize",
        [](const nb::bytes& bytes) { return SK::deserialize(bytes.c_str(), bytes.size()); },
        nb::arg("bytes"),
        "Deserializes the sketch from a bytes object."
    );
}

// nb::object and other types where the caller must provide a serde
template<typename T, typename SK, typename std::enable_if<!std::is_arithmetic<T>::value && !std::is_same<std::string, T>::value, bool>::type = 0>
void add_serialization(nb::class_<SK>& clazz) {
  clazz.def(
        "serialize",
        [](const SK& sk, datasketches::py_object_serde& serde) {
          auto bytes = sk.serialize(0, serde);
          return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        }, nb::arg("serde"),
        "Serializes the sketch into a bytes object using the provided serde."
    )
    .def_static(
        "deserialize",
        [](const nb::bytes& bytes, datasketches::py_object_serde& serde) {
            return SK::deserialize(bytes.c_str(), bytes.size(), serde);
        }, nb::arg("bytes"), nb::arg("serde"),
        "Deserializes the sketch from a bytes object using the provided serde."
    );
}

// Vector Updates
// * Only allowed for POD types based on numpy restriction, which
//   is equivalent to both std::is_trivial and std::is_standard_layout.
// * Nothing is added to types that are not PODs.
// POD type
template<typename T, typename SK, typename std::enable_if<std::is_trivial<T>::value && std::is_standard_layout<T>::value, bool>::type = 0>
void add_vector_update(nb::class_<SK>& clazz) {
  clazz.def(
    "update",
    [](SK& sk, nb::ndarray<T> items) {
      if (items.ndim() != 1) {
        throw std::invalid_argument("input data must have only one dimension. Found: "
          + std::to_string(items.ndim()));
      }
      auto v = items.template view<T, nb::ndim<1>>();
      for (uint32_t i = 0; i < v.shape(0); ++i) sk.update(v(i));
    },
    nb::arg("array"),
    "Updates the sketch with the values in the given array"
  );
}

// non-POD type
template<typename T, typename SK, typename std::enable_if<!std::is_trivial<T>::value || !std::is_standard_layout<T>::value, bool>::type = 0>
void add_vector_update(nb::class_<SK>& clazz) {
  unused(clazz);
}

#endif // _QUANTILE_CONDITIONAL_HPP_