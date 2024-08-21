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

#include <memory>
#include <string>
#include <nanobind/nanobind.h>
#include <nanobind/make_iterator.h>
#include <nanobind/intrusive/counter.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/string.h>

#include "py_serde.hpp"
#include "py_object_ostream.hpp"
#include "tuple_policy.hpp"

#include "theta_sketch.hpp"
#include "tuple_sketch.hpp"
#include "tuple_union.hpp"
#include "tuple_intersection.hpp"
#include "tuple_a_not_b.hpp"
#include "theta_jaccard_similarity_base.hpp"
#include "common_defs.hpp"

namespace nb = nanobind;

void init_tuple(nb::module_ &m) {
  using namespace datasketches;

  // generic tuple_policy:
  // * update sketch policy uses create_summary and update_summary
  // * set operation policies all use __call__
  nb::class_<tuple_policy, TuplePolicy>(m, "TuplePolicy",
      nb::intrusive_ptr<tuple_policy>(
      [](tuple_policy *tp, PyObject *po) noexcept { tp->set_self_py(po); }),
     "An abstract base class for Tuple Policy objects. All custom policies must extend this class.")
    .def(nb::init())
    .def("create_summary", &tuple_policy::create_summary,
         "Creates a new Summary object\n\n"
         ":return: a Summary object\n:rtype: :class:`object`"
         )
    .def("update_summary", &tuple_policy::update_summary, nb::arg("summary"), nb::arg("update"),
         "Applies the relevant policy to update the provided summary with the data in update.\n\n"
         ":param summary: An existing Summary\n:type summary: :class:`object`\n"
         ":param update: An update to apply to the Summary\n:type update: :class:`object`\n"
         ":return: The updated Summary\n:rtype: :class:`object`"
         )
    .def("__call__", &tuple_policy::operator(), nb::arg("summary"), nb::arg("update"),
         "Similar to update_summary but allows a different implementation for set operations (union and intersection)\n\n"
         ":param summary: An existing Summary\n:type summary: :class:`object`\n"
         ":param update: An update to apply to the Summary\n:type update: :class:`object`\n"
         ":return: The updated Summary\n:rtype: :class:`object`"
         )
  ;

  using py_tuple_sketch = tuple_sketch<nb::object>;
  using py_update_tuple = update_tuple_sketch<nb::object, nb::object, tuple_policy_holder>;
  using py_compact_tuple = compact_tuple_sketch<nb::object>;
  using py_tuple_union = tuple_union<nb::object, tuple_policy_holder>;
  using py_tuple_intersection = tuple_intersection<nb::object, tuple_policy_holder>;
  using py_tuple_a_not_b = tuple_a_not_b<nb::object>;
  using py_tuple_jaccard_similarity = jaccard_similarity_base<tuple_union<nb::object, dummy_jaccard_policy>, tuple_intersection<nb::object, dummy_jaccard_policy>, pair_extract_key<uint64_t, nb::object>>;

  nb::class_<py_tuple_sketch>(m, "tuple_sketch", "An abstract base class for tuple sketches.")
    .def("__str__", [](const py_tuple_sketch& sk) { return sk.to_string(); },
         "Produces a string summary of the sketch")
    .def("to_string", &py_tuple_sketch::to_string, nb::arg("print_items")=false,
         "Produces a string summary of the sketch")
    .def("is_empty", &py_tuple_sketch::is_empty,
         "Returns True if the sketch is empty, otherwise False")
    .def("get_estimate", &py_tuple_sketch::get_estimate,
         "Estimate of the distinct count of the input stream")
    .def("get_upper_bound", static_cast<double (py_tuple_sketch::*)(uint8_t) const>(&py_tuple_sketch::get_upper_bound), nb::arg("num_std_devs"),
         "Returns an approximate upper bound on the estimate at standard deviations in {1, 2, 3}")
    .def("get_lower_bound", static_cast<double (py_tuple_sketch::*)(uint8_t) const>(&py_tuple_sketch::get_lower_bound), nb::arg("num_std_devs"),
         "Returns an approximate lower bound on the estimate at standard deviations in {1, 2, 3}")
    .def("is_estimation_mode", &py_tuple_sketch::is_estimation_mode,
         "Returns True if sketch is in estimation mode, otherwise False")
    .def_prop_ro("theta", &py_tuple_sketch::get_theta,
         "Theta (effective sampling rate) as a fraction from 0 to 1")
    .def_prop_ro("theta64", &py_tuple_sketch::get_theta64,
         "Theta as 64-bit value")
    .def_prop_ro("num_retained", &py_tuple_sketch::get_num_retained,
         "The number of items currently in the sketch")
    .def("get_seed_hash", [](const py_tuple_sketch& sk) { return sk.get_seed_hash(); }, // why does regular call not work??
         "Returns a hash of the seed used in the sketch")
    .def("is_ordered", &py_tuple_sketch::is_ordered,
         "Returns True if the sketch entries are sorted, otherwise False")
    .def("__iter__",
          [](const py_tuple_sketch& s) {
               return nb::make_iterator(nb::type<py_tuple_sketch>(),
               "tuple_iterator",
               s.begin(),
               s.end());
          }, nb::keep_alive<0,1>()
     )
    .def_prop_ro_static("DEFAULT_SEED", [](nb::object /* self */) { return DEFAULT_SEED; });
  ;

  nb::class_<py_compact_tuple, py_tuple_sketch>(m, "compact_tuple_sketch")
    .def(nb::init<const py_tuple_sketch&, bool>(), nb::arg("other"), nb::arg("ordered")=true,
         "Creates a compact_tuple_sketch from an existing tuple_sketch.\n\n"
         ":param other: a sourch tuple_sketch\n:type other: tuple_sketch\n"
         ":param ordered: whether the incoming sketch entries are sorted. Default True\n"
         ":type ordered: bool, optional"
         )
    .def(nb::init<const theta_sketch&, nb::object&>(), nb::arg("other"), nb::arg("summary"),
         "Creates a compact_tuple_sketch from a theta_sketch using a fixed summary value.\n\n"
         ":param other: a source theta sketch\n:type other: theta_sketch\n"
         ":param summary: a summary to use for every sketch entry\n:type summary: object"
         )
    .def("__copy__", [](const py_compact_tuple& sk){ return py_compact_tuple(sk); })
    .def(
        "serialize",
        [](const py_compact_tuple& sk, py_object_serde& serde) {
          auto bytes = sk.serialize(0, serde);
          return nb::bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        }, nb::arg("serde"),
        "Serializes the sketch into a bytes object"
    )
    .def("filter",
         [](const py_compact_tuple& sk, const std::function<bool(const nb::object&)> func) {
           return sk.filter(func);
         }, nb::arg("predicate"),
         "Produces a compact_tuple_sketch from the given sketch by applying a predicate to "
         "the summary in each entry.\n\n"
         ":param predicate: A function returning true or value evaluated on each tuple summary\n"
         ":return: A compact_tuple_sketch with the selected entries\n:rtype: :class:`compact_tuple_sketch`")
    .def_static(
        "deserialize",
        [](const nb::bytes& bytes, py_object_serde& serde, uint64_t seed) {
          return py_compact_tuple::deserialize(bytes.c_str(), bytes.size(), seed, serde);
        },
        nb::arg("bytes"), nb::arg("serde"), nb::arg("seed")=DEFAULT_SEED,
        "Reads a bytes object and returns the corresponding compact_tuple_sketch"
    );

  nb::class_<py_update_tuple, py_tuple_sketch>(m, "update_tuple_sketch")
    .def("__init__",
        [](py_update_tuple* sk, tuple_policy* policy, uint8_t lg_k, double p, uint64_t seed) {
          tuple_policy_holder holder(policy);
          new (sk) py_update_tuple(py_update_tuple::builder(holder).set_lg_k(lg_k).set_p(p).set_seed(seed).build());
        },
        nb::arg("policy"), nb::arg("lg_k")=theta_constants::DEFAULT_LG_K, nb::arg("p")=1.0, nb::arg("seed")=DEFAULT_SEED,
        "Creates an update_tuple_sketch using the provided parameters\n\n"
        ":param policy: a policy to use when updating\n:type policy: TuplePolicy\n"
        ":param lg_k: base 2 logarithm of the maximum size of the sketch. Default 12.\n:type lg_k: int, optional\n"
        ":param p: an initial sampling rate to use. Default 1.0\n:type p: float, optional\n"
        ":param seed: the seed to use when hashing values\n:type seed: int, optional"
    )
    .def("__copy__", [](const py_update_tuple& sk){ return py_update_tuple(sk); })
    .def("update", static_cast<void (py_update_tuple::*)(int64_t, nb::object&)>(&py_update_tuple::update),
         nb::arg("datum"), nb::arg("value"),
         "Updates the sketch with the given integral item and summary value")
    .def("update", static_cast<void (py_update_tuple::*)(double, nb::object&)>(&py_update_tuple::update),
         nb::arg("datum"), nb::arg("value"),
         "Updates the sketch with the given floating point item and summary value")
    .def("update", static_cast<void (py_update_tuple::*)(const std::string&, nb::object&)>(&py_update_tuple::update),
         nb::arg("datum"), nb::arg("value"),
         "Updates the sketch with the given string item and summary value")
    .def("compact", &py_update_tuple::compact, nb::arg("ordered")=true,
         "Returns a compacted form of the sketch, optionally sorting it")
    .def("trim", &py_update_tuple::trim, "Removes retained entries in excess of the nominal size k (if any)")
    .def("reset", &py_update_tuple::reset, "Resets the sketch to the initial empty state")
    .def("filter",
         [](const py_update_tuple& sk, const std::function<bool(const nb::object&)> func) {
           return sk.filter(func);
         }, nb::arg("predicate"),
         "Produces a compact_tuple_sketch from the given sketch by applying a predicate to "
         "the summary in each entry.\n\n"
         ":param predicate: A function returning true or value evaluated on each tuple summary\n"
         ":return: A compact_tuple_sketch with the selected entries\n:rtype: :class:`compact_tuple_sketch`")
  ;

  nb::class_<py_tuple_union>(m, "tuple_union")
    .def("__init__",
        [](py_tuple_union* u, tuple_policy* policy, uint8_t lg_k, double p, uint64_t seed) {
          tuple_policy_holder holder(policy);
          new (u) py_tuple_union(py_tuple_union::builder(holder).set_lg_k(lg_k).set_p(p).set_seed(seed).build());
        },
        nb::arg("policy"), nb::arg("lg_k")=theta_constants::DEFAULT_LG_K, nb::arg("p")=1.0, nb::arg("seed")=DEFAULT_SEED,
        "Creates a tuple_union using the provided parameters\n\n"
        ":param policy: a policy to use when unioning entries\n:type policy: TuplePolicy\n"
        ":param lg_k: base 2 logarithm of the maximum size of the union. Default 12.\n:type lg_k: int, optional\n"
        ":param p: an initial sampling rate to use. Default 1.0\n:type p: float, optional\n"
        ":param seed: the seed to use when hashing values. Must match any sketch seeds.\n:type seed: int, optional"
    )
    .def("update", &py_tuple_union::update<const py_tuple_sketch&>, nb::arg("sketch"),
         "Updates the union with the given sketch")
    .def("get_result", &py_tuple_union::get_result, nb::arg("ordered")=true,
         "Returns the sketch corresponding to the union result")
    .def("reset", &py_tuple_union::reset,
         "Resets the sketch to the initial empty")
  ;

  nb::class_<py_tuple_intersection>(m, "tuple_intersection")
    .def("__init__",
        [](py_tuple_intersection* sk, tuple_policy* policy, uint64_t seed) {
          tuple_policy_holder holder(policy);
          new (sk) py_tuple_intersection(seed, holder);
        },
        nb::arg("policy"), nb::arg("seed")=DEFAULT_SEED,
        "Creates a tuple_intersection using the provided parameters\n\n"
        ":param policy: a policy to use when intersecting entries\n:type policy: TuplePolicy\n"
        ":param seed: the seed to use when hashing values. Must match any sketch seeds\n:type seed: int, optional"
    )
    .def("update", &py_tuple_intersection::update<const py_tuple_sketch&>, nb::arg("sketch"),
         "Intersects the provided sketch with the current intersection state")
    .def("get_result", &py_tuple_intersection::get_result, nb::arg("ordered")=true,
         "Returns the sketch corresponding to the intersection result")
    .def("has_result", &py_tuple_intersection::has_result,
         "Returns True if the intersection has a valid result, otherwise False")
  ;

  nb::class_<py_tuple_a_not_b>(m, "tuple_a_not_b")
    .def(nb::init<uint64_t>(), nb::arg("seed")=DEFAULT_SEED,
        "Creates a tuple_a_not_b object\n\n"
        ":param seed: the seed to use when hashing values. Must match any sketch seeds.\n:type seed: int, optional"
    )
    .def(
        "compute",
        &py_tuple_a_not_b::compute<const py_tuple_sketch&, const py_tuple_sketch&>,
        nb::arg("a"), nb::arg("b"), nb::arg("ordered")=true,
        "Returns a sketch with the result of applying the A-not-B operation on the given inputs"
    )
  ;

  nb::class_<py_tuple_jaccard_similarity>(m, "tuple_jaccard_similarity",
     "An object to help compute Jaccard similarity between tuple sketches.")
    .def_static(
        "jaccard",
        [](const py_tuple_sketch& sketch_a, const py_tuple_sketch& sketch_b, uint64_t seed) {
          return py_tuple_jaccard_similarity::jaccard(sketch_a, sketch_b, seed);
        },
        nb::arg("sketch_a"), nb::arg("sketch_b"), nb::arg("seed")=DEFAULT_SEED,
        "Returns a list with {lower_bound, estimate, upper_bound} of the Jaccard similarity between sketches"
    )
    .def_static(
        "exactly_equal",
        &py_tuple_jaccard_similarity::exactly_equal<const py_tuple_sketch&, const py_tuple_sketch&>,
        nb::arg("sketch_a"), nb::arg("sketch_b"), nb::arg("seed")=DEFAULT_SEED,
        "Returns True if sketch_a and sketch_b are equivalent, otherwise False"
    )
    .def_static(
        "similarity_test",
        &py_tuple_jaccard_similarity::similarity_test<const py_tuple_sketch&, const py_tuple_sketch&>,
        nb::arg("actual"), nb::arg("expected"), nb::arg("threshold"), nb::arg("seed")=DEFAULT_SEED,
        "Tests similarity of an actual sketch against an expected sketch. Computes the lower bound of the Jaccard "
        "index J_{LB} of the actual and expected sketches. If J_{LB} >= threshold, then the sketches are considered "
        "to be similar with a confidence of 97.7% and returns True, otherwise False.")
    .def_static(
        "dissimilarity_test",
        &py_tuple_jaccard_similarity::dissimilarity_test<const py_tuple_sketch&, const py_tuple_sketch&>,
        nb::arg("actual"), nb::arg("expected"), nb::arg("threshold"), nb::arg("seed")=DEFAULT_SEED,
        "Tests dissimilarity of an actual sketch against an expected sketch. Computes the upper bound of the Jaccard "
        "index J_{UB} of the actual and expected sketches. If J_{UB} <= threshold, then the sketches are considered "
        "to be dissimilar with a confidence of 97.7% and returns True, otherwise False."
    )
  ;
}
