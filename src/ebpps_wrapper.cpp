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

#include "ebpps_sketch.hpp"
#include "py_serde.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace datasketches {

namespace python {

template<typename T>
ebpps_sketch<T> ebpps_sketch_deserialize(py::bytes& skBytes, py_object_serde& sd) {
  std::string skStr = skBytes; // implicit cast
  return ebpps_sketch<T>::deserialize(skStr.c_str(), skStr.length(), sd);
}

template<typename T>
py::object ebpps_sketch_serialize(const ebpps_sketch<T>& sk, py_object_serde& sd) {
  auto serResult = sk.serialize(0, sd);
  return py::bytes((char*)serResult.data(), serResult.size());
}

template<typename T>
std::string ebpps_sketch_to_string(const ebpps_sketch<T>& sk, bool print_items) {
  if (print_items) {
    std::ostringstream ss;
    ss << sk.to_string();
    ss << "### EBPPS Sketch Items" << std::endl;
    int i = 0;
    for (auto item : sk) {
      // item is an arbitrary py::object, so get the value by
      // using internal str() method then casting to C++ std::string
      py::str item_pystr(item);
      std::string item_str = py::cast<std::string>(item_pystr);
      ss << i++ << ": " << item_str << std::endl;
    }
    return ss.str();
  } else {
    return sk.to_string();
  }
}

}
}

namespace dspy = datasketches::python;

template<typename T>
void bind_ebpps_sketch(py::module &m, const char* name) {
  using namespace datasketches;

  py::class_<ebpps_sketch<T>>(m, name)
    .def(py::init<uint32_t>(), py::arg("k"))
    .def("__str__", &dspy::ebpps_sketch_to_string<T>, py::arg("print_items")=false,
         "Produces a string summary of the sketch")
    .def("to_string", &dspy::ebpps_sketch_to_string<T>, py::arg("print_items")=false,
         "Produces a string summary of the sketch")
    .def("update", (void (ebpps_sketch<T>::*)(const T&, double)) &ebpps_sketch<T>::update, py::arg("item"), py::arg("weight")=1.0,
         "Updates the sketch with the given value and weight")
    .def("merge", (void (ebpps_sketch<T>::*)(const ebpps_sketch<T>&)) &ebpps_sketch<T>::merge,
         py::arg("sketch"), "Merges the sketch with the given sketch")
    .def_property_readonly("k", &ebpps_sketch<T>::get_k,
         "Returns the sketch's maximum configured sample size")
    .def_property_readonly("n", &ebpps_sketch<T>::get_n,
         "Returns the total stream length")         
    .def_property_readonly("c", &ebpps_sketch<T>::get_c,
         "Returns the expected number of samples returned upon a call to get_result() or the creation of an iterator. "
         "The number is a floating point value, where the fractional portion represents the probability of including "
         "a \"partial item\" from the sample. The value C should be no larger than the sketch's configured value of k, "
         "although numerical precision limitations mean it may exceed k by double precision floating point error margins in certain cases.")
    .def("get_samples", &ebpps_sketch<T>::get_result,
         "Returns the set of samples in the sketch")
    .def("is_empty", &ebpps_sketch<T>::is_empty,
         "Returns True if the sketch is empty, otherwise False")
    .def("get_serialized_size_bytes",
         [](const ebpps_sketch<T>& sk, py_object_serde& sd) { return sk.get_serialized_size_bytes(sd); },
         py::arg("serde"),
        "Computes the size in bytes needed to serialize the current sketch")
    .def("serialize", &dspy::ebpps_sketch_serialize<T>, py::arg("serde"), "Serialize the var opt sketch using the provided serde")
    .def_static("deserialize", &dspy::ebpps_sketch_deserialize<T>, py::arg("bytes"), py::arg("serde"),
        "Constructs a var opt sketch from the given bytes using the provided serde")
    .def("__iter__", [](const ebpps_sketch<T>& sk) { return py::make_iterator(sk.begin(), sk.end()); });
}

void init_ebpps(py::module &m) {
  bind_ebpps_sketch<py::object>(m, "ebpps_sketch");
}
