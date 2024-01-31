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

#include <functional>
#include <string>
#include <sstream>
#include <stdexcept>
#include <variant>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/variant.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/string.h>

#include "kll_sketch.hpp"

namespace nb = nanobind;

namespace datasketches {

namespace vector_of_kll_constants {
  static const uint32_t DEFAULT_K = kll_constants::DEFAULT_K;
  static const uint32_t DEFAULT_D = 1;
}

// Wrapper class for Numpy compatibility
template <typename T, typename C = std::less<T>>
class vector_of_kll_sketches {
  public:
    explicit vector_of_kll_sketches(uint32_t k = vector_of_kll_constants::DEFAULT_K, uint32_t d = vector_of_kll_constants::DEFAULT_D);
    vector_of_kll_sketches(const vector_of_kll_sketches& other);
    vector_of_kll_sketches(vector_of_kll_sketches&& other) noexcept;
    vector_of_kll_sketches<T, C>& operator=(const vector_of_kll_sketches& other);
    vector_of_kll_sketches<T, C>& operator=(vector_of_kll_sketches&& other);

    // container parameters
    inline uint32_t get_k() const;
    inline uint32_t get_d() const;

    template<typename V>
    using Array1D = nb::ndarray<V, nb::numpy, nb::ndim<1>>;

    template<typename V>
    using Array2D = nb::ndarray<V, nb::numpy, nb::ndim<2>, nb::c_contig>;

    // sketch updates/merges
    void update(nb::ndarray<T>& items, char order);
    void merge(const vector_of_kll_sketches<T>& other);

    template<typename V>
    using ArrInputType = std::variant<nb::ndarray<>, nb::list, V>;

    // returns a single sketch combining all data in the array
    kll_sketch<T, C> collapse(ArrInputType<int>& isk) const;

    // sketch queries returning an array of results
    Array1D<bool> is_empty() const;
    Array1D<uint64_t> get_n() const;
    Array1D<bool> is_estimation_mode() const;
    Array1D<T> get_min_values() const;
    Array1D<T> get_max_values() const;
    Array1D<uint32_t> get_num_retained() const;
    Array2D<T> get_quantiles(ArrInputType<double>& ranks, ArrInputType<int>& isk) const;
    Array2D<double> get_ranks(ArrInputType<T>& values, ArrInputType<int>& isk) const;
    Array2D<double> get_pmf(ArrInputType<T>& split_points, ArrInputType<int>& isk) const;
    Array2D<double> get_cdf(ArrInputType<T>& split_points, ArrInputType<int>& isk) const;

    // human-readable output
    std::string to_string(bool print_levels = false, bool print_items = false) const;

    // binary output/input
    nb::list serialize(ArrInputType<int>& isk);
    // note: deserialize() replaces the sketch at the specified
    //       index. Not a static method.
    void deserialize(const nb::bytes& sk_bytes, uint32_t idx);

  private:
    template<typename TT>
    Array1D<TT> input_to_vec(ArrInputType<TT>& input) const;

    Array1D<uint32_t> get_indices(Array1D<int>& isk) const;
    
    template<typename TT>
    Array1D<TT> make_ndarray(size_t size) const;

    template<typename TT>
    Array2D<TT> make_ndarray(size_t rows, size_t cols) const;

    const uint32_t k_; // kll sketch k parameter
    const uint32_t d_; // number of dimensions (here: sketches) to hold
    std::vector<kll_sketch<T, C>> sketches_;
};

template<typename T, typename C>
vector_of_kll_sketches<T, C>::vector_of_kll_sketches(uint32_t k, uint32_t d):
k_(k), 
d_(d)
{
  // check d is valid (k is checked by kll_sketch)
  if (d < 1) {
    throw std::invalid_argument("D must be >= 1: " + std::to_string(d));
  }

  sketches_.reserve(d);
  // spawn the sketches
  for (uint32_t i = 0; i < d; i++) {
    sketches_.emplace_back(k);
  }
}

template<typename T, typename C>
vector_of_kll_sketches<T, C>::vector_of_kll_sketches(const vector_of_kll_sketches& other) :
  k_(other.k_),
  d_(other.d_),
  sketches_(other.sketches_)
{}

template<typename T, typename C>
vector_of_kll_sketches<T, C>::vector_of_kll_sketches(vector_of_kll_sketches&& other) noexcept :
  k_(other.k_),
  d_(other.d_),
  sketches_(std::move(other.sketches_))
{}

template<typename T, typename C>
vector_of_kll_sketches<T, C>& vector_of_kll_sketches<T, C>::operator=(const vector_of_kll_sketches& other) {
  vector_of_kll_sketches<T, C> copy(other);
  k_ = copy.k_;
  d_ = copy.d_;
  std::swap(sketches_, copy.sketches_);
  return *this;
}

template<typename T, typename C>
vector_of_kll_sketches<T, C>& vector_of_kll_sketches<T, C>::operator=(vector_of_kll_sketches&& other) {
  k_ = other.k_;
  d_ = other.d_;
  std::swap(sketches_, other.sketches_);
  return *this;
}

template<typename T, typename C>
uint32_t vector_of_kll_sketches<T, C>::get_k() const {
  return k_;
}

template<typename T, typename C>
uint32_t vector_of_kll_sketches<T, C>::get_d() const {
  return d_;
}

template<typename T, typename C>
template<typename TT>
auto vector_of_kll_sketches<T, C>::make_ndarray(size_t size) const -> Array1D<TT> {
  TT* data = new TT[size];

  nb::capsule owner(data, [](void *p) noexcept {
    delete[] static_cast<TT*>(p);
  });

  return Array1D<TT>(data, {size}, owner);
}

template<typename T, typename C>
template<typename TT>
auto vector_of_kll_sketches<T, C>::make_ndarray(size_t rows, size_t cols) const -> Array2D<TT> {
  TT* data = new TT[rows * cols];

  nb::capsule owner(data, [](void *p) noexcept {
    delete[] static_cast<TT*>(p);
  });

  return Array2D<TT>(data, {rows, cols}, owner);
}

template<typename T, typename C>
template<typename TT>
auto vector_of_kll_sketches<T, C>::input_to_vec(ArrInputType<TT>& input) const -> Array1D<TT> {
  if (std::holds_alternative<nb::ndarray<>>(input)) {
    nb::ndarray<> arr = std::get<nb::ndarray<>>(input);
    return Array1D<TT>(arr);
  } else if (std::holds_alternative<nb::list>(input)) {
    nb::list list = std::get<nb::list>(input);
    Array1D<TT> output = make_ndarray<TT>(list.size());
    for (size_t i = 0; i < list.size(); ++i)
      output(i) = nb::cast<TT>(list[i]);
    return output;
  } else if (std::holds_alternative<TT>(input)) {
    Array1D<TT> output = make_ndarray<TT>(1);
    output(0) = std::get<TT>(input);
    return output;
  } else {
    nb::raise_type_error("Unknown input type");
  }
}

template<typename T, typename C>
auto vector_of_kll_sketches<T, C>::get_indices(Array1D<int>& isk) const -> Array1D<uint32_t> {
  auto input = isk.view<nb::ndim<1>>();
  size_t num_input = input.shape(0);
  Array1D<uint32_t> output;

  if (num_input == 1) {
    if (input(0) == -1) {
      output = make_ndarray<uint32_t>(d_);
      for (uint32_t i = 0; i < d_; ++i) {
        output(i) = i;
      }
    } else {
      output = make_ndarray<uint32_t>(1);
      output(0) = static_cast<uint32_t>(input(0));
    }
  } else {
    output = make_ndarray<uint32_t>(num_input);
    for (uint32_t i = 0; i < num_input; ++i) {
      const uint32_t idx = static_cast<uint32_t>(input(i));
      if (idx < d_) {
        output(i) = idx;
      } else {
        throw std::invalid_argument("request for invalid dimensions >= d ("
                 + std::to_string(d_) +"): "+ std::to_string(idx));
      }
    }
  }
  return output;
}

// Checks if each sketch is empty or not
template<typename T, typename C>
auto vector_of_kll_sketches<T, C>::is_empty() const -> Array1D<bool> {
  auto vals = make_ndarray<bool>(d_);
  auto view = vals.view();
  for (uint32_t i = 0; i < d_; ++i) {
    view(i) = sketches_[i].is_empty();
  }
  return vals;
}

// Updates each sketch with values
// Currently: all values must be present
// TODO: allow subsets of sketches to be updated
template<typename T, typename C>
void vector_of_kll_sketches<T, C>::update(nb::ndarray<T>& items, char order) {
  size_t ndim = items.ndim();

  if (items.shape(ndim-1) != d_) {
    throw std::invalid_argument("input data must have rows with  " + std::to_string(d_)
          + " elements. Found: " + std::to_string(items.shape(ndim-1)));
  }
  
  if (ndim == 1) {
    // 1D case: single value to update per sketch
    const T* data = items.data();
    for (uint32_t i = 0; i < d_; ++i) {
      sketches_[i].update(data[i]);
    }
  }
  else if (ndim == 2) {
    // 2D case: multiple values to update per sketch
    // We could speedthis up by using raw array access and pre-computing an offset for the
    // row/column, but if we use the wrong ordering the computation would be incorrect.
    // By using a view and dereferencing by (row, column) each time we ensure correct
    // processing at the cost of an extra multiply each derference. Using a mismatched
    // ordering versus the actual data storage will be potentially slower but will still
    // produce correct output.
    auto data = items.template view<nb::ndim<2>>();
    if (order == 'F' || order == 'f') { // Fortran-style (column-major) order
      for (uint32_t j = 0; j < d_; ++j) {
        const size_t offset = j * d_;
        for (uint32_t i = 0; i < items.shape(0); ++i) { 
          sketches_[j].update(data(i, j));
        }
      }
    } else { // nb::c_contig or nb::any_contig
      for (uint32_t i = 0; i < items.shape(0); ++i) { 
        const size_t offset = i * items.shape(0);
        for (uint32_t j = 0; j < d_; ++j) {
          sketches_[j].update(data(i, j));
        }
      }
    }
  }
  else {
    throw std::invalid_argument("Update input must be 2 or fewer dimensions : " + std::to_string(ndim));
  }
}

// Merges two arrays of sketches
// Currently: all values must be present
template<typename T, typename C>
void vector_of_kll_sketches<T, C>::merge(const vector_of_kll_sketches<T>& other) {
  if (d_ != other.get_d()) {
    throw std::invalid_argument("Must have same number of dimensions to merge: " + std::to_string(d_)
                                + " vs " + std::to_string(other.d_));
  } else {
    for (uint32_t i = 0; i < d_; ++i) {
      sketches_[i].merge(other.sketches_[i]);
    }
  }
}

template<typename T, typename C>
kll_sketch<T, C> vector_of_kll_sketches<T, C>::collapse(ArrInputType<int>& isk) const {
  Array1D<int> indices = input_to_vec<int>(isk);
  Array1D<uint32_t> index_arr = get_indices(indices);
  auto inds = index_arr.view();
  
  kll_sketch<T, C> result(k_);
  for (size_t idx = 0; idx < inds.shape(0); ++idx) {
    result.merge(sketches_[inds(idx)]);
  }
  return result;
}

// Number of updates for each sketch
template<typename T, typename C>
auto vector_of_kll_sketches<T, C>::get_n() const -> Array1D<uint64_t> {
  auto vals = make_ndarray<uint64_t>(d_);
  auto view = vals.view();
  for (uint32_t i = 0; i < d_; ++i) {
    view(i) = sketches_[i].get_n();
  }
  return vals;
}

// Number of retained values for each sketch
template<typename T, typename C>
auto vector_of_kll_sketches<T, C>::get_num_retained() const -> Array1D<uint32_t> {
  auto vals = make_ndarray<uint32_t>(d_);
  auto view = vals.view();
  for (uint32_t i = 0; i < d_; ++i) {
    view(i) = sketches_[i].get_num_retained();
  }
  return vals;
}

// Gets the minimum value of each sketch
// TODO: allow subsets of sketches
template<typename T, typename C>
auto vector_of_kll_sketches<T, C>::get_min_values() const -> Array1D<T> {
  //std::vector<T> vals(d_);
  auto vals = make_ndarray<T>(d_);
  auto view = vals.view();
  for (uint32_t i = 0; i < d_; ++i) {
    view(i) = sketches_[i].get_min_item();
  }
  return vals;
}

// Gets the maximum value of each sketch
// TODO: allow subsets of sketches
template<typename T, typename C>
auto vector_of_kll_sketches<T, C>::get_max_values() const -> Array1D<T> {
  auto vals = make_ndarray<T>(d_);
  auto view = vals.view();
  for (uint32_t i = 0; i < d_; ++i) {
    view(i) = sketches_[i].get_max_item();
  }
  return vals;
}

// Summary of each sketch as one long string
// Users should use .split('\n\n') when calling it to build a list of each 
// sketch's summary
template<typename T, typename C>
std::string vector_of_kll_sketches<T, C>::to_string(bool print_levels, bool print_items) const {
  std::ostringstream ss;
  for (uint32_t i = 0; i < d_; ++i) {
    // all streams into 1 string, for compatibility with Python's str() behavior
    // users will need to split by \n\n, e.g., str(kll).split('\n\n')
    if (i > 0) ss << "\n";
    ss << sketches_[i].to_string(print_levels, print_items);
  }
  return ss.str();
}

template<typename T, typename C>
auto vector_of_kll_sketches<T, C>::is_estimation_mode() const -> Array1D<bool> {
  auto vals = make_ndarray<bool>(d_);
  auto view = vals.view();
  for (uint32_t i = 0; i < d_; ++i) {
    view(i) = sketches_[i].is_estimation_mode();
  }
  return vals;
}

// Value of sketch(es) corresponding to some quantile(s)
template<typename T, typename C>
auto vector_of_kll_sketches<T, C>::get_quantiles(ArrInputType<double>& ranks,
                                                 ArrInputType<int>& isk) const -> Array2D<T> {
  Array1D<int> indices = input_to_vec<int>(isk);
  Array1D<uint32_t> inds = get_indices(indices);
  size_t num_sketches = inds.size();

  Array1D<double> ranks_arr = input_to_vec<double>(ranks);
  size_t num_quantiles = ranks_arr.size();

  auto quants = make_ndarray<T>(num_sketches, num_quantiles);
  auto view = quants.view();
  auto ranks_view = ranks_arr.view();
  for (uint32_t i = 0; i < num_sketches; ++i) {
    for (size_t j = 0; j < num_quantiles; ++j) {
      view(i, j) = sketches_[inds(i)].get_quantile(ranks_view(j));
    }
  }
  return quants;
}

// Value of sketch(es) corresponding to some rank(s)
template<typename T, typename C>
auto vector_of_kll_sketches<T, C>::get_ranks(ArrInputType<T>& values,
                                             ArrInputType<int>& isk) const -> Array2D<double> {
  Array1D<int> indices = input_to_vec<int>(isk);
  Array1D<uint32_t> inds = get_indices(indices);
  size_t num_sketches = inds.size();
  
  Array1D<T> values_arr = input_to_vec<T>(values);
  size_t num_ranks = values_arr.size();
  auto vals = values_arr.view();

  auto ranks = make_ndarray<double>(num_sketches, num_ranks);
  auto view = ranks.view();
  for (uint32_t i = 0; i < num_sketches; ++i) {
    const size_t offset = i * num_ranks;
    for (size_t j = 0; j < num_ranks; ++j) {
      view(i, j) = sketches_[inds(i)].get_rank(vals(j));
    }
  }
  return ranks;
}

// PMF(s) of sketch(es)
template<typename T, typename C>
auto vector_of_kll_sketches<T, C>::get_pmf(ArrInputType<T>& split_points,
                                           ArrInputType<int>& isk) const -> Array2D<double> {
  Array1D<int> indices = input_to_vec<int>(isk);  
  Array1D<uint32_t> inds = get_indices(indices);
  size_t num_sketches = inds.size();

  Array1D<T> splits_arr = input_to_vec<T>(split_points);
  size_t num_splits = splits_arr.size();
  
  auto pmfs = make_ndarray<double>(num_sketches, num_splits + 1);
  auto view = pmfs.view();
  for (uint32_t i = 0; i < num_sketches; ++i) {
    auto pmf = sketches_[inds(i)].get_PMF(splits_arr.data(), num_splits);
    for (size_t j = 0; j <= num_splits; ++j) {
      view(i, j) = pmf[j];
    }
  }
  return pmfs;
}

// CDF(s) of sketch(es)
template<typename T, typename C>
auto vector_of_kll_sketches<T, C>::get_cdf(ArrInputType<T>& split_points,
                                           ArrInputType<int>& isk) const -> Array2D<double> {
  Array1D<int> indices = input_to_vec<int>(isk);
  Array1D<uint32_t> inds = get_indices(indices);
  size_t num_sketches = inds.size();

  Array1D<T> splits_arr = input_to_vec<T>(split_points);
  size_t num_splits = splits_arr.size();

  auto cdfs = make_ndarray<double>(num_sketches, num_splits + 1);
  auto view = cdfs.view();
  for (uint32_t i = 0; i < num_sketches; ++i) {
    auto cdf = sketches_[inds(i)].get_CDF(splits_arr.data(), num_splits);
    for (size_t j = 0; j <= num_splits; ++j) {
      view(i, j) = cdf[j];
    }
  }
  return cdfs;
}

template<typename T, typename C>
void vector_of_kll_sketches<T, C>::deserialize(const nb::bytes& sk_bytes,
                                               uint32_t idx) {
  if (idx >= d_) {
    throw std::invalid_argument("request for invalid dimensions >= d ("
             + std::to_string(d_) +"): "+ std::to_string(idx));
  }
  // load the sketch into the proper index
  sketches_[idx] = std::move(kll_sketch<T>::deserialize(sk_bytes.c_str(), sk_bytes.size()));
}

template<typename T, typename C>
nb::list vector_of_kll_sketches<T, C>::serialize(ArrInputType<int>& isk) {
  Array1D<int> indices = input_to_vec<int>(isk);
  Array1D<uint32_t> inds = get_indices(indices);
  const size_t num_sketches = inds.size();

  nb::list list;
  for (uint32_t i = 0; i < num_sketches; ++i) {
    auto serResult = sketches_[inds(i)].serialize();
    list.append(nb::bytes((char*)serResult.data(), serResult.size()));
  }

  return list;
}

} // namespace datasketches

template<typename T>
void bind_vector_of_kll_sketches(nb::module_ &m, const char* name) {
  using namespace datasketches;

  nb::class_<vector_of_kll_sketches<T>>(m, name)
    .def(nb::init<uint32_t, uint32_t>(), nb::arg("k")=vector_of_kll_constants::DEFAULT_K, 
                                         nb::arg("d")=vector_of_kll_constants::DEFAULT_D,
         "Creates a new Vector of KLL Sketches instance with the given values of k and d.\n\n"
         ":param k: The value of k for every sketch in the vector\n:type k: int\n"
         ":param d: The number of sketches in the vector\n:type d: int"
        )
    .def("__copy__", [](const vector_of_kll_sketches<T>& sk){ return vector_of_kll_sketches<T>(sk); })
    // allow user to retrieve k or d, in case it's instantiated w/ defaults
    .def_prop_ro("k", &vector_of_kll_sketches<T>::get_k,
         "The value of `k` of the sketch(es)")
    .def_prop_ro("d", &vector_of_kll_sketches<T>::get_d,
         "The number of sketches")
    .def("update", &vector_of_kll_sketches<T>::update, nb::arg("items"), nb::arg("order") = "C",
         "Updates the sketch(es) with value(s).  Must be a 1D array of size equal to the number of sketches.  Can also be 2D array of shape (n_updates, n_sketches).  If a sketch does not have a value to update, use np.nan. "
         " Order 'F' specifies a column-major (Fortran style) matrix; any other value assumes row-major (C style) matrix.")
    .def("__str__", [](const vector_of_kll_sketches<T>& sk) { return sk.to_string(); },
         "Produces a string summary of all sketches. Users should split the returned string by '\\n\\n'")
    .def("to_string", &vector_of_kll_sketches<T>::to_string, nb::arg("print_levels")=false,
                                                             nb::arg("print_items")=false,
         "Produces a string summary of all sketches. Users should split the returned string by '\\n\\n'")
    .def("is_empty", &vector_of_kll_sketches<T>::is_empty,
         "Returns whether the sketch(es) is(are) empty of not")
    .def("get_n", &vector_of_kll_sketches<T>::get_n, 
         "Returns the number of values seen by the sketch(es)")
    .def("get_num_retained", &vector_of_kll_sketches<T>::get_num_retained, 
         "Returns the number of values retained by the sketch(es)")
    .def("is_estimation_mode", &vector_of_kll_sketches<T>::is_estimation_mode, 
         "Returns whether the sketch(es) is(are) in estimation mode")
    .def("get_min_values", &vector_of_kll_sketches<T>::get_min_values,
         "Returns the minimum value(s) of the sketch(es)")
    .def("get_max_values", &vector_of_kll_sketches<T>::get_max_values,
         "Returns the maximum value(s) of the sketch(es)")
    .def("get_quantiles", &vector_of_kll_sketches<T>::get_quantiles, nb::arg("ranks"),
                                                                     nb::arg("isk")=-1, 
         "Returns the value(s) associated with the specified quantile(s) for the specified sketch(es). `ranks` can be a float between 0 and 1 (inclusive), or a list/array of values. `isk` specifies which sketch(es) to return the value(s) for (default: all sketches)")
    .def("get_ranks", &vector_of_kll_sketches<T>::get_ranks,
         nb::arg("value"), nb::arg("isk")=-1, 
         "Returns the value(s) associated with the specified rank(s) for the specified sketch(es). `values` can be an int between 0 and the number of values retained, or a list/array of values. `isk` specifies which sketch(es) to return the value(s) for (default: all sketches)")
    .def("get_pmf", &vector_of_kll_sketches<T>::get_pmf, nb::arg("split_points"), nb::arg("isk")=-1, 
         "Returns the probability mass function (PMF) at `split_points` of the specified sketch(es).  `split_points` should be a list/array of floats between 0 and 1 (inclusive). `isk` specifies which sketch(es) to return the PMF for (default: all sketches)")
    .def("get_cdf", &vector_of_kll_sketches<T>::get_cdf, nb::arg("split_points"), nb::arg("isk")=-1, 
         "Returns the cumulative distribution function (CDF) at `split_points` of the specified sketch(es).  `split_points` should be a list/array of floats between 0 and 1 (inclusive). `isk` specifies which sketch(es) to return the CDF for (default: all sketches)")
    .def_static("get_normalized_rank_error",
        [](uint16_t k, bool pmf) { return kll_sketch<T>::get_normalized_rank_error(k, pmf); },
         nb::arg("k"), nb::arg("as_pmf"), "Returns the normalized rank error")
    .def("serialize", &vector_of_kll_sketches<T>::serialize, nb::arg("isk")=-1, 
         "Serializes the specified sketch(es). `isk` can be an int or a list/array of ints (default: all sketches)")
    .def("deserialize", &vector_of_kll_sketches<T>::deserialize, nb::arg("skBytes"), nb::arg("isk"), 
         "Deserializes the specified sketch.  `isk` must be an int.")
    .def("merge", &vector_of_kll_sketches<T>::merge, nb::arg("array_of_sketches"),
         "Merges the input array of KLL sketches into the existing array.")
    .def("collapse", &vector_of_kll_sketches<T>::collapse, nb::arg("isk")=-1,
         "Returns the result of collapsing all sketches in the array into a single sketch.  'isk' can be an int or a list/array of ints (default: all sketches)")
    ;
}

void init_vector_of_kll(nb::module_ &m) {
  bind_vector_of_kll_sketches<int>(m, "vector_of_kll_ints_sketches");
  bind_vector_of_kll_sketches<float>(m, "vector_of_kll_floats_sketches");
}
