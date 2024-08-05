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
#include <nanobind/intrusive/counter.h>

// needed for sketches such as Density and Tuple which rely
// on a joint C++/Python object
#include <nanobind/intrusive/counter.inl>

namespace nb = nanobind;

// sketches
void init_hll(nb::module_& m);
void init_kll(nb::module_& m);
void init_fi(nb::module_& m);
void init_cpc(nb::module_& m);
void init_theta(nb::module_& m);
void init_tuple(nb::module_& m);
void init_vo(nb::module_& m);
void init_ebpps(nb::module_& m);
void init_req(nb::module_& m);
void init_quantiles(nb::module_& m);
void init_count_min(nb::module_& m);
void init_density(nb::module_& m);
void init_tdigest(nb::module_& m);
void init_vector_of_kll(nb::module_& m);

// supporting objects
void init_kolmogorov_smirnov(nb::module_& m);
void init_serde(nb::module_& m);

NB_MODULE(_datasketches, m) {
  // needed in conjunction with the counter.inl include above
  nb::intrusive_init(
    [](PyObject *o) noexcept {
        nb::gil_scoped_acquire guard;
        Py_INCREF(o);
    },
    [](PyObject *o) noexcept {
        nb::gil_scoped_acquire guard;
        Py_DECREF(o);
    }
  );

  init_hll(m);
  init_kll(m);
  init_fi(m);
  init_cpc(m);
  init_theta(m);
  init_tuple(m);
  init_vo(m);
  init_ebpps(m);
  init_req(m);
  init_quantiles(m);
  init_count_min(m);
  init_density(m);
  init_tdigest(m);
  init_vector_of_kll(m);

  init_kolmogorov_smirnov(m);
  init_serde(m);
}
