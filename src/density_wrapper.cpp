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

#include <vector>
#include <memory>
#include <nanobind/nanobind.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/vector.h>
#include <nanobind/make_iterator.h>

#include <nanobind/eval.h>

#include "kernel_function.hpp"
#include "density_sketch.hpp"

namespace nb = nanobind;

template<typename T, typename K>
void bind_density_sketch(nb::module_ &m, const char* name) {
  using namespace datasketches;

  nb::class_<density_sketch<T, K>>(m, name)
    .def("__init__", [](density_sketch<T, K>* sk, uint16_t k, uint32_t dim, std::shared_ptr<kernel_function> kernel)
        { K holder(kernel);
          new (sk) density_sketch<T, K>(k, dim, holder);
        },
        nb::arg("k"), nb::arg("dim"), nb::arg("kernel"))
    .def("update", static_cast<void (density_sketch<T, K>::*)(const std::vector<T>&)>(&density_sketch<T, K>::update), nb::arg("vector"),
        "Updates the sketch with the given vector")
    .def("merge", static_cast<void (density_sketch<T, K>::*)(const density_sketch<T, K>&)>(&density_sketch<T, K>::merge), nb::arg("sketch"),
        "Merges the provided sketch into this one")
    .def("is_empty", &density_sketch<T, K>::is_empty,
        "Returns True if the sketch is empty, otherwise False")
    .def("get_k", &density_sketch<T, K>::get_k,
        "Returns the configured parameter k")
    .def("get_dim", &density_sketch<T, K>::get_dim,
        "Returns the configured parameter dim")
    .def("get_n", &density_sketch<T, K>::get_n,
        "Returns the length of the input stream")
    .def("get_num_retained", &density_sketch<T, K>::get_num_retained,
        "Returns the number of retained items (samples) in the sketch")
    .def("is_estimation_mode", &density_sketch<T, K>::is_estimation_mode,
        "Returns True if the sketch is in estimation mode, otherwise False")
    .def("get_estimate", &density_sketch<T, K>::get_estimate, nb::arg("point"),
        "Returns an approximate density at the given point")
    .def("__str__", &density_sketch<T, K>::to_string, nb::arg("print_levels")=false, nb::arg("print_items")=false,
        "Produces a string summary of the sketch")
    .def("to_string", &density_sketch<T, K>::to_string, nb::arg("print_levels")=false, nb::arg("print_items")=false,
        "Produces a string summary of the sketch")
    .def("__iter__", [](const density_sketch<T, K> &sk) {
                        return nb::make_iterator(nb::type<density_sketch<T,K> >(),
                                                 "density_iterator",
                                                 sk.begin(),
                                             sk.end());
                      },
        nb::keep_alive<0,1>())
    .def("serialize",
        [](const density_sketch<T, K>& sk) {
          auto bytes = sk.serialize();
          return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        },
        "Serializes the sketch into a bytes object"
    )
    .def_static(
        "deserialize",
        [](const nb::bytes& bytes, std::shared_ptr<kernel_function> kernel) {
          K holder(kernel);
          return density_sketch<T, K>::deserialize(bytes.c_str(), bytes.size(), holder);
        },
        nb::arg("bytes"), nb::arg("kernel"),
        "Reads a bytes object and returns the corresponding density_sketch"
    );
}

void init_density(nb::module_ &m) {
  using namespace datasketches;

  // generic kernel function
  nb::class_<kernel_function, KernelFunction>(m, "KernelFunction")
    .def(nb::init())
    .def("__call__", &kernel_function::operator(), nb::arg("a"), nb::arg("b"))
    ;

  // the old sketch names can almost be defined, but the kernel_function_holder won't work in init()
  //bind_density_sketch<float, gaussian_kernel<float>>(m, "density_floats_sketch");
  //bind_density_sketch<double, gaussian_kernel<double>>(m, "density_doubles_sketch");
  bind_density_sketch<double, kernel_function_holder>(m, "_density_sketch");
}
