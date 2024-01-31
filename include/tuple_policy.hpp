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
#include <nanobind/nanobind.h>
#include <nanobind/trampoline.h>
#include <nanobind/intrusive/counter.h>
#include <nanobind/intrusive/ref.h>

#ifndef _TUPLE_POLICY_HPP_
#define _TUPLE_POLICY_HPP_

namespace nb = nanobind;

namespace datasketches {

/**
 * @brief tuple_policy provides the underlying base class from
 *        which native Python policies ultimately inherit. The actual
 *        policies implement TuplePolicy, as shown in TuplePolicy.py
 */
struct tuple_policy : public nb::intrusive_base {
  virtual nb::object create_summary() const = 0;
  virtual nb::object update_summary(nb::object& summary, const nb::object& update) const = 0;
  virtual nb::object operator()(nb::object& summary, const nb::object& update) const = 0;
  virtual ~tuple_policy() = default;
};

/**
 * @brief TuplePolicy provides the "trampoline" class for nanobind
 *        that allows for a native Python implementation of tuple
 *        sketch policies.
 */
struct TuplePolicy : public tuple_policy {
  NB_TRAMPOLINE(tuple_policy, 3);

  /**
   * @brief Create a summary object
   * 
   * @return nb::object representing a new summary
   */
  nb::object create_summary() const override {
    NB_OVERRIDE_PURE(
      create_summary,      // Name of function in C++ (must match Python name)
                           // Argument(s) -- if any
    );
  }

  /**
   * @brief Update a summary object using this policy
   * 
   * @param summary The current summary to update
   * @param update The new value with which to update the summary
   * @return nb::object The updated summary
   */
  nb::object update_summary(nb::object& summary, const nb::object& update) const override {
    NB_OVERRIDE_PURE(
      update_summary,      // Name of function in C++ (must match Python name)
      summary, update      // Arguments
    );
  }

  /**
   * @brief Applies this policy to summary with the provided update
   * 
   * @param summary The current summary on which to apply the policy
   * @param update An update to apply to the current summary
   * @return nb::object The potentially modified summary
   */
  nb::object operator()(nb::object& summary, const nb::object& update) const override {
    NB_OVERRIDE_PURE_NAME(
      "__call__",          // Name of function in python
      operator(),          // Name of function in C++
      summary, update      // Arguments
    );
  }
};

/* The tuple_policy_holder provides a concrete class that dispatches calls
 * from the sketch to the tuple_policy. This class is needed to provide a
 * concrete object to produce a compiled library, but library users should
 * never need to use this directly.
 */
struct tuple_policy_holder {
  explicit tuple_policy_holder(tuple_policy* policy) : _policy(policy) {}
  tuple_policy_holder(const tuple_policy_holder& other) : _policy(other._policy) {}
  tuple_policy_holder(tuple_policy_holder&& other) : _policy(std::move(other._policy)) {}
  tuple_policy_holder& operator=(const tuple_policy_holder& other) { _policy = other._policy; return *this; }
  tuple_policy_holder& operator=(tuple_policy_holder&& other) { std::swap(_policy, other._policy); return *this; }

  nb::object create() const { return _policy->create_summary(); }
  
  void update(nb::object& summary, const nb::object& update) const {
    summary = _policy->update_summary(summary, update);
  }

  void operator()(nb::object& summary, const nb::object& update) const {
    summary = _policy->operator()(summary, update);
  }

  private:
    nb::ref<tuple_policy> _policy;
};

/* A degenerate policy used to enable Jaccard Similarity on tuple sketches,
 * where the computation requires a union and intersection over the keys but
 * does not need to observe the summaries.
 */
struct dummy_jaccard_policy {
  void operator()(nb::object&, const nb::object&) const {
    return;
  }
};

}

#endif // _TUPLE_POLICY_HPP_