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
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "py_serde.hpp"
#include "py_object_ostream.hpp"

#include "ebpps_sketch.hpp"

namespace nb = nanobind;

template<typename T>
void bind_ebpps_sketch(nb::module_ &m, const char* name) {
  using namespace datasketches;

  nb::class_<ebpps_sketch<T>>(m, name)
    .def(nb::init<uint32_t>(), nb::arg("k"),
         "Creates a new EBPPS sketch instance\n\n"
         ":param k: Maximum number of samples in the sketch\n:type k: int\n"
         )
    .def("__copy__", [](const ebpps_sketch<T>& sk){ return ebpps_sketch<T>(sk); })
    .def("__str__", [](const ebpps_sketch<T>& sk) { return sk.to_string(); },
         "Produces a string summary of the sketch")
    .def("to_string",
         [](const ebpps_sketch<T>& sk, bool print_items) {
           std::ostringstream ss;
           ss << sk.to_string();
           if (print_items)
             ss << sk.items_to_string();
           return ss.str();
         }, nb::arg("print_items")=false,
         "Produces a string summary of the sketch and optionally prints the items")
    .def("update", (void (ebpps_sketch<T>::*)(const T&, double)) &ebpps_sketch<T>::update, nb::arg("item"), nb::arg("weight")=1.0,
         "Updates the sketch with the given value and weight")
    .def("merge", (void (ebpps_sketch<T>::*)(const ebpps_sketch<T>&)) &ebpps_sketch<T>::merge,
         nb::arg("sketch"), "Merges the sketch with the given sketch")
    .def_prop_ro("k", &ebpps_sketch<T>::get_k,
         "The sketch's maximum configured sample size")
    .def_prop_ro("n", &ebpps_sketch<T>::get_n,
         "The total stream length")         
    .def_prop_ro("c", &ebpps_sketch<T>::get_c,
         "The expected number of samples returned upon a call to get_result() or the creation of an iterator. "
         "The number is a floating point value, where the fractional portion represents the probability of including "
         "a \"partial item\" from the sample. The value C should be no larger than the sketch's configured value of k, "
         "although numerical precision limitations mean it may exceed k by double precision floating point error margins in certain cases.")
    .def("is_empty", &ebpps_sketch<T>::is_empty,
         "Returns True if the sketch is empty, otherwise False")
    .def("get_serialized_size_bytes",
         [](const ebpps_sketch<T>& sk, py_object_serde& serde) { return sk.get_serialized_size_bytes(serde); },
         nb::arg("serde"),
         "Computes the size in bytes needed to serialize the current sketch")
    .def("serialize",
         [](const ebpps_sketch<T>& sk, py_object_serde& serde) {
           auto bytes = sk.serialize(0, serde);
           return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
         }, nb::arg("serde"),
         "Serializes the sketch into a bytes object")
    .def_static(
         "deserialize",
         [](const nb::bytes& bytes, py_object_serde& serde) { return ebpps_sketch<T>::deserialize(bytes.c_str(), bytes.size(), serde); },
         nb::arg("bytes"), nb::arg("serde"),
         "Reads a bytes object and returns the corresponding ebpps_sketch")
    .def("__iter__",
          [](const ebpps_sketch<T>& sk) {
               return nb::make_iterator(nb::type<ebpps_sketch<T>>(),
               "ebpps_iterator",
               sk.begin(),
               sk.end());
          }, nb::keep_alive<0,1>()
     )
     ;
}

void init_ebpps(nb::module_ &m) {
  bind_ebpps_sketch<nb::object>(m, "ebpps_sketch");
}
