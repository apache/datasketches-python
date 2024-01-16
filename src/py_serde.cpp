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

#include <cstring>
#include "memory_operations.hpp"

#include "py_serde.hpp"

#include <nanobind/nanobind.h>

namespace nb = nanobind;

void init_serde(nb::module_& m) {
  using namespace datasketches;
  nb::class_<py_object_serde, PyObjectSerDe /* <--- trampoline*/>(m, "PyObjectSerDe",
    "An abstract base class for serde objects. All custom serdes must extend this class.")
    .def(nb::init<>())
    .def("get_size", &py_object_serde::get_size, nb::arg("item"),
        "Returns the size in bytes of an item\n\n"
        ":param item: The specified object\n:type item: object\n"
        ":return: The size of the item in bytes\n:rtype: int"
        )
    .def("to_bytes", &py_object_serde::to_bytes, nb::arg("item"),
        "Retuns a bytes object with a serialized version of an item\n\n"
        ":param item: The specified object\n:type item: object\n"
        ":return: A :class:`bytes` object with the serialized object\n:rtype: bytes"
        )
    .def("from_bytes", &py_object_serde::from_bytes, nb::arg("data"), nb::arg("offset"),
        "Reads a bytes object starting from the given offest and returns a tuple of the reconstructed "
        "object and the number of additional bytes read\n\n"
        ":param data: A :class:`bytes` object from which to deserialize\n:type data: bytes\n"
        ":param offset: The offset, in bytes, at which to start reading\n:type offset: int\n"
        ":return: A :class:`tuple` with the reconstructed object and the number of bytes read\n"
        ":rtype: tuple(object, int)"
        )
    ;
}    

namespace datasketches {
  size_t py_object_serde::size_of_item(const nb::object& item) const {
    return get_size(item);
  }

  size_t py_object_serde::serialize(void* ptr, size_t capacity, const nb::object* items, unsigned num) const {
    size_t bytes_written = 0;
    nb::gil_scoped_acquire acquire;
    for (unsigned i = 0; i < num; ++i) {
      nb::bytes bytes = to_bytes(items[i]); // implicit cast from nb::bytes
      check_memory_size(bytes_written + bytes.size(), capacity);
      memcpy(ptr, bytes.c_str(), bytes.size());
      ptr = static_cast<char*>(ptr) + bytes.size();
      bytes_written += bytes.size();
    }
    nb::gil_scoped_release release;
    return bytes_written;
  }

  size_t py_object_serde::deserialize(const void* ptr, size_t capacity, nb::object* items, unsigned num) const {
    size_t bytes_read = 0;
    unsigned i = 0;
    bool failure = false;
    bool error_from_python = false;
    nb::gil_scoped_acquire acquire;

    // copy data into bytes only once
    nb::bytes bytes(static_cast<const char*>(ptr), capacity);
    for (; i < num && !failure; ++i) {
      nb::tuple bytes_and_len;
      try {
        bytes_and_len = from_bytes(bytes, bytes_read);
      } catch (nb::python_error &e) {
        failure = true;
        error_from_python = true;
        break;
      }

      size_t length = nb::cast<size_t>(bytes_and_len[1]);
      if (bytes_read + length > capacity) {
        bytes_read += length; // use this value to report the error
        failure = true;
        break;
      }
      
      new (&items[i]) nb::object(nb::cast<nb::object>(bytes_and_len[0]));
      ptr = static_cast<const char*>(ptr) + length;
      bytes_read += length;
    }

    if (failure) {
      // clean up what we've allocated
      for (unsigned j = 0; j < i; ++j) {
        items[j].dec_ref();
      }

      if (error_from_python) {
        throw nb::value_error("Error reading value in from_bytes");
      } else {
        // this next call will throw
        check_memory_size(bytes_read, capacity);
      }
    }

    nb::gil_scoped_release release;
    return bytes_read;
  }


} // namespace datasketches