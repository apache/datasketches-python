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


#include "py_serde.hpp"
#include "py_object_ostream.hpp"
#include "frequent_items_sketch.hpp"

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/string.h>

#include <exception>
#include <ostream>

namespace pb = nanobind;

// forward declarations
// std::string and arithmetic types, where we don't need a separate serde
template<typename T, typename W, typename H, typename E, typename std::enable_if<std::is_arithmetic<T>::value || std::is_same<std::string, T>::value, bool>::type = 0>
void add_serialization(nb::class_<datasketches::frequent_items_sketch<T, W, H, E>>& clazz);

// nb::object and other types where the caller must provide a serde
template<typename T, typename W, typename H, typename E, typename std::enable_if<!std::is_arithmetic<T>::value && !std::is_same<std::string, T>::value, bool>::type = 0>
void add_serialization(nb::class_<datasketches::frequent_items_sketch<T, W, H, E>>& clazz);

template<typename T, typename W, typename H, typename E>
void bind_fi_sketch(nb::module_ &m, const char* name) {
  using namespace datasketches;

  auto fi_class = nb::class_<frequent_items_sketch<T, W, H, E>>(m, name)
    .def(nb::init<uint8_t>(), nb::arg("lg_max_k"),
         "Creates an instance of the sketch\n\n"
         ":param lg_max_k: base 2 logarithm of the maximum size of the internal hash map of the sketch. Maximum "
         "capacity is 0.75 of this value, which is the maximum number of distinct items the sketch can contain.\n"
         ":type lg_max_k: int\n"
         )
    .def("__copy__", [](const frequent_items_sketch<T, W, H, E>& sk){ return frequent_items_sketch<T,W,H,E>(sk); })
    .def("__str__", [](const frequent_items_sketch<T, W, H, E>& sk) { return sk.to_string(); },
         "Produces a string summary of the sketch")
    .def("to_string", &frequent_items_sketch<T, W, H, E>::to_string, nb::arg("print_items")=false,
         "Produces a string summary of the sketch")
    .def("update", (void (frequent_items_sketch<T, W, H, E>::*)(const T&, uint64_t)) &frequent_items_sketch<T, W, H, E>::update, nb::arg("item"), nb::arg("weight")=1,
         "Updates the sketch with the given string and, optionally, a weight")
    .def("merge", (void (frequent_items_sketch<T, W, H, E>::*)(const frequent_items_sketch<T, W, H, E>&)) &frequent_items_sketch<T, W, H, E>::merge,
         "Merges the given sketch into this one")
    .def("is_empty", &frequent_items_sketch<T, W, H, E>::is_empty,
         "Returns True if the sketch is empty, otherwise False")
    .def_prop_ro("num_active_items", &frequent_items_sketch<T, W, H, E>::get_num_active_items,
         "The number of active items in the sketch")
    .def_prop_ro("total_weight", &frequent_items_sketch<T, W, H, E>::get_total_weight,
         "The sum of the weights (frequencies) in the stream seen so far by the sketch")
    .def("get_estimate", &frequent_items_sketch<T, W, H, E>::get_estimate, nb::arg("item"),
         "Returns the estimate of the weight (frequency) of the given item.\n"
         "Note: The true frequency of a item would be the sum of the counts as a result of the "
         "two update functions.")
    .def("get_lower_bound", &frequent_items_sketch<T, W, H, E>::get_lower_bound, nb::arg("item"),
         "Returns the guaranteed lower bound weight (frequency) of the given item.")
    .def("get_upper_bound", &frequent_items_sketch<T, W, H, E>::get_upper_bound, nb::arg("item"),
         "Returns the guaranteed upper bound weight (frequency) of the given item.")
    .def_prop_ro("epsilon", (double (frequent_items_sketch<T, W, H, E>::*)(void) const) &frequent_items_sketch<T, W, H, E>::get_epsilon,
         "The epsilon value used by the sketch to compute error")
    .def(
        "get_frequent_items",
        [](const frequent_items_sketch<T, W, H, E>& sk, frequent_items_error_type err_type, uint64_t threshold) {
          if (threshold == 0) threshold = sk.get_maximum_error();
          nb::list list;
          auto rows = sk.get_frequent_items(err_type, threshold);
          for (auto row: rows) {
            list.append(nb::make_tuple(
                row.get_item(),
                row.get_estimate(),
                row.get_lower_bound(),
                row.get_upper_bound())
            );
          }
          return list;
        },
        nb::arg("err_type"), nb::arg("threshold")=0
    )
    .def_static(
        "get_epsilon_for_lg_size",
        [](uint8_t lg_max_map_size) { return frequent_items_sketch<T, W, H, E>::get_epsilon(lg_max_map_size); },
        nb::arg("lg_max_map_size"),
        "Returns the epsilon value used to compute a priori error for a given log2(max_map_size)"
    )
    .def_static(
        "get_apriori_error",
        &frequent_items_sketch<T, W, H, E>::get_apriori_error,
        nb::arg("lg_max_map_size"), nb::arg("estimated_total_weight"),
        "Returns the estimated a priori error given the max_map_size for the sketch and the estimated_total_stream_weight."
    );

    // serialization may need a caller-provided serde depending on the sketch type, so
    // we use a separate method to handle that appropriately based on type T.
    add_serialization(fi_class);
}

// std::string or arithmetic types, for which we have a built-in serde
template<typename T, typename W, typename H, typename E, typename std::enable_if<std::is_arithmetic<T>::value || std::is_same<std::string, T>::value, bool>::type>
void add_serialization(nb::class_<datasketches::frequent_items_sketch<T, W, H, E>>& clazz) {
    using namespace datasketches;
    clazz.def(
        "get_serialized_size_bytes",
        [](const frequent_items_sketch<T, W, H, E>& sk) { return sk.get_serialized_size_bytes(); },
        "Computes the size needed to serialize the current state of the sketch. This can be expensive since every item needs to be looked at."
    )
    .def(
        "serialize",
        [](const frequent_items_sketch<T, W, H, E>& sk) {
          auto bytes = sk.serialize();
          return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        },
        "Serializes the sketch into a bytes object."
    )
    .def_static(
        "deserialize",
        [](const nb::bytes& bytes) { return frequent_items_sketch<T, W, H, E>::deserialize(bytes.c_str(), bytes.size()); },
        nb::arg("bytes"),
        "Reads a bytes object and returns the corresponding frequent_strings_sketch."
    );
}

// nb::object or any other type that requires a provided serde
template<typename T, typename W, typename H, typename E, typename std::enable_if<!std::is_arithmetic<T>::value && !std::is_same<std::string, T>::value, bool>::type>
void add_serialization(nb::class_<datasketches::frequent_items_sketch<T, W, H, E>>& clazz) {
    using namespace datasketches;
    clazz.def(
        "get_serialized_size_bytes",
        [](const frequent_items_sketch<T, W, H, E>& sk, py_object_serde& serde) { return sk.get_serialized_size_bytes(serde); },
        nb::arg("serde"),
        "Computes the size needed to serialize the current state of the sketch using the provided serde. This can be expensive since every item needs to be looked at."
    )
    .def(
        "serialize",
        [](const frequent_items_sketch<T, W, H, E>& sk, py_object_serde& serde) {
          auto bytes = sk.serialize(0, serde);
          return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        }, nb::arg("serde"),
        "Serializes the sketch into a bytes object using the provided serde."
    )
    .def_static(
        "deserialize",
        [](const nb::bytes& bytes, py_object_serde& serde) {
          return frequent_items_sketch<T, W, H, E>::deserialize(bytes.c_str(), bytes.size(), serde);
        }, nb::arg("bytes"), nb::arg("serde"),
        "Reads a bytes object using the provided serde and returns the corresponding frequent_strings_sketch."
    );
}

// calls class __hash__ method
struct py_hash_caller {
  virtual size_t operator()(const nb::object& a) const {
    Py_hash_t result = PyObject_Hash(a.ptr());
    if (result == -1) {
      throw nb::type_error("Could not compute hash value of object");
    }
    return static_cast<size_t>(result);
  }
};

// calls class __eq__ method
struct py_equal_caller {
  bool operator()(const nb::object& a, const nb::object& b) const {
    return a.equal(b);
  }
};

void init_fi(nb::module_ &m) {
  using namespace datasketches;

  nb::enum_<frequent_items_error_type>(m, "frequent_items_error_type")
    .value("NO_FALSE_POSITIVES", NO_FALSE_POSITIVES, "Returns only true positives, but may miss some heavy hitters.")
    .value("NO_FALSE_NEGATIVES", NO_FALSE_NEGATIVES, "Does not miss any heavy hitters, but may return false positives.")
    .export_values();

  bind_fi_sketch<std::string, uint64_t, std::hash<std::string>, std::equal_to<std::string>>(m, "frequent_strings_sketch");
  bind_fi_sketch<nb::object, uint64_t, py_hash_caller, py_equal_caller>(m, "frequent_items_sketch"); 
}
