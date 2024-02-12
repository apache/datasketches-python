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

#include <string>
#include <nanobind/nanobind.h>
#include <nanobind/make_iterator.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/string.h>

#include "theta_sketch.hpp"
#include "theta_union.hpp"
#include "theta_intersection.hpp"
#include "theta_a_not_b.hpp"
#include "theta_jaccard_similarity.hpp"
#include "common_defs.hpp"

namespace nb = nanobind;

void init_theta(nb::module_ &m) {
  using namespace datasketches;

  nb::class_<theta_sketch>(m, "theta_sketch", "An abstract base class for theta sketches")
    .def("__str__", [](const theta_sketch& sk) { return sk.to_string(); },
         "Produces a string summary of the sketch")
    .def("to_string", [](const theta_sketch& sk, bool print_items) { return sk.to_string(print_items); }, nb::arg("print_items")=false,
         "Produces a string summary of the sketch")
    .def("is_empty", static_cast<bool (theta_sketch::*)() const>(&theta_sketch::is_empty),
         "Returns True if the sketch is empty, otherwise False")
    .def("get_estimate", static_cast<double (theta_sketch::*)() const>(&theta_sketch::get_estimate),
         "Estimate of the distinct count of the input stream")
    .def("get_upper_bound", static_cast<double (theta_sketch::*)(uint8_t) const>(&theta_sketch::get_upper_bound), nb::arg("num_std_devs"),
         "Returns an approximate upper bound on the estimate at standard deviations in {1, 2, 3}")
    .def("get_lower_bound", static_cast<double (theta_sketch::*)(uint8_t) const>(&theta_sketch::get_lower_bound), nb::arg("num_std_devs"),
         "Returns an approximate lower bound on the estimate at standard deviations in {1, 2, 3}")
    .def("is_estimation_mode", static_cast<bool (theta_sketch::*)() const>(&theta_sketch::is_estimation_mode),
         "Returns True if sketch is in estimation mode, otherwise False")
    .def_prop_ro("theta", static_cast<double (theta_sketch::*)() const>(&theta_sketch::get_theta),
         "Theta (effective sampling rate) as a fraction from 0 to 1")
    .def_prop_ro("theta64", static_cast<uint64_t (theta_sketch::*)() const>(&theta_sketch::get_theta64),
         "Theta as 64-bit value")
    .def_prop_ro("num_retained", static_cast<uint32_t (theta_sketch::*)() const>(&theta_sketch::get_num_retained),
         "The number of items currently in the sketch")
    .def("get_seed_hash", static_cast<uint16_t (theta_sketch::*)() const>(&theta_sketch::get_seed_hash),
         "Returns a hash of the seed used in the sketch")
    .def("is_ordered", static_cast<bool (theta_sketch::*)() const>(&theta_sketch::is_ordered),
         "Returns True if the sketch entries are sorted, otherwise False")
    .def("__iter__",
          [](const theta_sketch& s) {
               return nb::make_iterator(nb::type<theta_sketch>(),
               "theta_iterator",
               s.begin(),
               s.end());
          }, nb::keep_alive<0,1>()
     )
  ;

  nb::class_<update_theta_sketch, theta_sketch>(m, "update_theta_sketch")
    .def("__init__",
        [](update_theta_sketch* sk, uint8_t lg_k, double p, uint64_t seed) {
          new (sk) update_theta_sketch(update_theta_sketch::builder().set_lg_k(lg_k).set_p(p).set_seed(seed).build());
        },
        nb::arg("lg_k")=theta_constants::DEFAULT_LG_K, nb::arg("p")=1.0, nb::arg("seed")=DEFAULT_SEED,
        "Creates an update_theta_sketch using the provided parameters\n\n"
        ":param lg_k: base 2 logarithm of the maximum size of the sketch. Default 12.\n:type lg_k: int, optional\n"
        ":param p: an initial sampling rate to use. Default 1.0\n:type p: float, optional\n"
        ":param seed: the seed to use when hashing values\n:type seed: int, optional\n"
    )
    .def("__copy__", [](const update_theta_sketch& sk){ return update_theta_sketch(sk); })
    .def("update", (void (update_theta_sketch::*)(int64_t)) &update_theta_sketch::update, nb::arg("datum"),
         "Updates the sketch with the given integral value")
    .def("update", (void (update_theta_sketch::*)(double)) &update_theta_sketch::update, nb::arg("datum"),
         "Updates the sketch with the given floating point value")
    .def("update", (void (update_theta_sketch::*)(const std::string&)) &update_theta_sketch::update, nb::arg("datum"),
         "Updates the sketch with the given string")
    .def("compact", &update_theta_sketch::compact, nb::arg("ordered")=true,
         "Returns a compacted form of the sketch, optionally sorting it")
    .def("trim", &update_theta_sketch::trim, "Removes retained entries in excess of the nominal size k (if any)")
    .def("reset", &update_theta_sketch::reset, "Resets the sketch to the initial empty state")
  ;

  nb::class_<compact_theta_sketch, theta_sketch>(m, "compact_theta_sketch")
    .def(nb::init<const theta_sketch&, bool>(),
         "Creates a compact_theta_sketch from an existing theta_sketch.\n\n"
         ":param other: a source theta_sketch\n:type other: theta_sketch\n"
         ":param ordered: whether the incoming sketch entries are sorted. Default True\n"
         ":type ordered: bool"
    )
    .def("__copy__", [](const compact_theta_sketch& sk){ return compact_theta_sketch(sk); })
    .def(
        "serialize",
        [](const compact_theta_sketch& sk, bool compress) {
          auto bytes = compress ? sk.serialize_compressed() : sk.serialize();
          return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        }, nb::arg("compress")=false,
        "Serializes the sketch into a bytes object, optionally compressing the data"
    )
    .def_static(
        "deserialize",
        [](const nb::bytes& bytes, uint64_t seed) {
          return compact_theta_sketch::deserialize(bytes.c_str(), bytes.size(), seed);
        },
        nb::arg("bytes"), nb::arg("seed")=DEFAULT_SEED,
        "Reads a bytes object and returns the corresponding compact_theta_sketch"
    );

  nb::class_<theta_union>(m, "theta_union")
    .def("__init__",
        [](theta_union* u, uint8_t lg_k, double p, uint64_t seed) {
          new (u) theta_union(theta_union::builder().set_lg_k(lg_k).set_p(p).set_seed(seed).build());
        },
        nb::arg("lg_k")=theta_constants::DEFAULT_LG_K, nb::arg("p")=1.0, nb::arg("seed")=DEFAULT_SEED,
        "Creates a theta_union using the provided parameters\n\n"
        ":param lg_k: base 2 logarithm of the maximum size of the union. Default 12.\n:type lg_k: int, optional\n"
        ":param p: an initial sampling rate to use. Default 1.0\n:type p: float, optional\n"
        ":param seed: the seed to use when hashing values. Must match all sketch seeds.\n:type seed: int, optional"
    )
    .def("update", &theta_union::update<const theta_sketch&>, nb::arg("sketch"),
         "Updates the union with the given sketch")
    .def("get_result", &theta_union::get_result, nb::arg("ordered")=true,
         "Returns the sketch corresponding to the union result")
  ;

  nb::class_<theta_intersection>(m, "theta_intersection")
    .def(nb::init<uint64_t>(), nb::arg("seed")=DEFAULT_SEED,
        "Creates a theta_intersection using the provided parameters\n\n"
        ":param seed: the seed to use when hashing values. Must match all sketch seeds\n:type seed: int, optional"         
    )
    .def("update", &theta_intersection::update<const theta_sketch&>, nb::arg("sketch"),
         "Intersections the provided sketch with the current intersection state")
    .def("get_result", &theta_intersection::get_result, nb::arg("ordered")=true,
         "Returns the sketch corresponding to the intersection result")
    .def("has_result", &theta_intersection::has_result,
         "Returns True if the intersection has a valid result, otherwise False")
  ;

  nb::class_<theta_a_not_b>(m, "theta_a_not_b")
    .def(nb::init<uint64_t>(), nb::arg("seed")=DEFAULT_SEED,
        "Creates a tuple_a_not_b object\n\n"
        ":param seed: the seed to use when hashing values. Must match all sketch seeds.\n:type seed: int, optional"
    )
    .def(
        "compute",
        &theta_a_not_b::compute<const theta_sketch&, const theta_sketch&>,
        nb::arg("a"), nb::arg("b"), nb::arg("ordered")=true,
        "Returns a sketch with the result of applying the A-not-B operation on the given inputs"
    )
  ;
  
  nb::class_<theta_jaccard_similarity>(m, "theta_jaccard_similarity",
    "An object to help compute Jaccard similarity between theta sketches.")
    .def_static(
        "jaccard",
        [](const theta_sketch& sketch_a, const theta_sketch& sketch_b, uint64_t seed) {
          return theta_jaccard_similarity::jaccard(sketch_a, sketch_b, seed);
        },
        nb::arg("sketch_a"), nb::arg("sketch_b"), nb::arg("seed")=DEFAULT_SEED,
        "Returns a list with {lower_bound, estimate, upper_bound} of the Jaccard similarity between sketches"
    )
    .def_static(
        "exactly_equal",
        &theta_jaccard_similarity::exactly_equal<const theta_sketch&, const theta_sketch&>,
        nb::arg("sketch_a"), nb::arg("sketch_b"), nb::arg("seed")=DEFAULT_SEED,
        "Returns True if sketch_a and sketch_b are equivalent, otherwise False"
    )
    .def_static(
        "similarity_test",
        &theta_jaccard_similarity::similarity_test<const theta_sketch&, const theta_sketch&>,
        nb::arg("actual"), nb::arg("expected"), nb::arg("threshold"), nb::arg("seed")=DEFAULT_SEED,
        "Tests similarity of an actual sketch against an expected sketch. Computers the lower bound of the Jaccard "
        "index J_{LB} of the actual and expected sketches. If J_{LB} >= threshold, then the sketches are considered "
        "to be similar with a confidence of 97.7% and returns True, otherwise False.")
    .def_static(
        "dissimilarity_test",
        &theta_jaccard_similarity::dissimilarity_test<const theta_sketch&, const theta_sketch&>,
        nb::arg("actual"), nb::arg("expected"), nb::arg("threshold"), nb::arg("seed")=DEFAULT_SEED,
        "Tests dissimilarity of an actual sketch against an expected sketch. Computers the lower bound of the Jaccard "
        "index J_{UB} of the actual and expected sketches. If J_{UB} <= threshold, then the sketches are considered "
        "to be dissimilar with a confidence of 97.7% and returns True, otherwise False."
    )
  ;     
}
