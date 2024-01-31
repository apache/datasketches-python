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
#include <nanobind/make_iterator.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/string.h>

#include "py_serde.hpp"
#include "py_object_ostream.hpp"

#include "var_opt_sketch.hpp"
#include "var_opt_union.hpp"

namespace nb = nanobind;

template<typename T>
void bind_vo_sketch(nb::module_ &m, const char* name) {
  using namespace datasketches;

  nb::class_<var_opt_sketch<T>>(m, name)
    .def(nb::init<uint32_t>(), nb::arg("k"),
         "Creates a new Var Opt sketch instance\n\n"
         ":param k: Maximum number of samples in the sketch\n:type k: int\n"
    )
    .def("__copy__", [](const var_opt_sketch<T>& sk){ return var_opt_sketch<T>(sk); })
    .def("__str__", [](const var_opt_sketch<T>& sk) { return sk.to_string(); },
         "Produces a string summary of the sketch")
    .def("to_string",
         [](const var_opt_sketch<T>& sk, bool print_items) {
           std::ostringstream ss;
           ss << sk.to_string();
           if (print_items)
             ss << sk.items_to_string();
           return ss.str();
         }, nb::arg("print_items")=false,
         "Produces a string summary of the sketch and optionally prints the items")
    .def("update", (void (var_opt_sketch<T>::*)(const T&, double)) &var_opt_sketch<T>::update, nb::arg("item"), nb::arg("weight")=1.0,
         "Updates the sketch with the given value and weight")
    .def_prop_ro("k", &var_opt_sketch<T>::get_k,
         "Returns the sketch's maximum configured sample size")
    .def_prop_ro("n", &var_opt_sketch<T>::get_n,
         "Returns the total stream length")
    .def_prop_ro("num_samples", &var_opt_sketch<T>::get_num_samples,
         "Returns the number of samples currently in the sketch")
    .def("is_empty", &var_opt_sketch<T>::is_empty,
         "Returns True if the sketch is empty, otherwise False")
    .def("estimate_subset_sum",
         [](const var_opt_sketch<T>& sk, const std::function<bool(T)> func) {
           subset_summary summary = sk.estimate_subset_sum(func);
           nb::dict d;
           d["estimate"] = summary.estimate;
           d["lower_bound"] = summary.lower_bound;
           d["upper_bound"] = summary.upper_bound;
           d["total_sketch_weight"] = summary.total_sketch_weight;
           return d;
         }, nb::arg("predicate"),
         "Applies a provided predicate to the sketch and returns the estimated total weight matching the predicate, as well "
         "as upper and lower bounds on the estimate and the total weight processed by the sketch")
    .def("get_serialized_size_bytes",
         [](const var_opt_sketch<T>& sk, py_object_serde& serde) { return sk.get_serialized_size_bytes(serde); },
         nb::arg("serde"),
         "Computes the size in bytes needed to serialize the current sketch")
    .def("serialize",
         [](const var_opt_sketch<T>& sk, py_object_serde& serde) {
           auto bytes = sk.serialize(0, serde);
           return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
         }, nb::arg("serde"),
         "Serializes the sketch into a bytes object")
    .def_static(
         "deserialize",
         [](const nb::bytes& bytes, py_object_serde& serde) { return var_opt_sketch<T>::deserialize(bytes.c_str(), bytes.size(), serde); },
         nb::arg("bytes"), nb::arg("serde"),
         "Reads a bytes object and returns the corresponding var opt sketch")
    .def("__iter__",
          [](const var_opt_sketch<T>& sk) {
               return nb::make_iterator(nb::type<var_opt_sketch<T>>(),
               "var_opt_iterator",
               sk.begin(),
               sk.end());
          }, nb::keep_alive<0,1>()
     )
     ;
}

template<typename T>
void bind_vo_union(nb::module_ &m, const char* name) {
  using namespace datasketches;

  nb::class_<var_opt_union<T>>(m, name)
    .def(nb::init<uint32_t>(), nb::arg("max_k"))
    .def("__str__", [](const var_opt_union<T>& sk) { return sk.to_string(); },
         "Produces a string summary of the sketch")
    .def("to_string", &var_opt_union<T>::to_string,
         "Produces a string summary of the sketch")
    .def("update", (void (var_opt_union<T>::*)(const var_opt_sketch<T>& sk)) &var_opt_union<T>::update, nb::arg("sketch"),
         "Updates the union with the given sketch")
    .def("get_result", &var_opt_union<T>::get_result,
         "Returns a sketch corresponding to the union result")
    .def("reset", &var_opt_union<T>::reset,
         "Resets the union to the empty state")
    .def("get_serialized_size_bytes",
         [](const var_opt_union<T>& u, py_object_serde& serde) { return u.get_serialized_size_bytes(serde); },
         nb::arg("serde"),
         "Computes the size in bytes needed to serialize the current union")
    .def("serialize",
         [](const var_opt_union<T>& u, py_object_serde& serde) {
           auto bytes = u.serialize(0, serde);
           return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
         }, nb::arg("serde"),
         "Serializes the union into a bytes object with the provided serde")
    .def_static(
         "deserialize",
         [](const nb::bytes& bytes, py_object_serde& serde) { return var_opt_union<T>::deserialize(bytes.c_str(), bytes.size(), serde); },
         nb::arg("bytes"), nb::arg("serde"),
         "Constructs a var opt union from the given bytes using the provided serde")
    ;
}

void init_vo(nb::module_ &m) {
  bind_vo_sketch<nb::object>(m, "var_opt_sketch");
  bind_vo_union<nb::object>(m, "var_opt_union");
}
