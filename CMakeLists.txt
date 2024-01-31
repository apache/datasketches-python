# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

cmake_minimum_required(VERSION 3.16.0)

string(TIMESTAMP DT %Y%m%d UTC)
string(TIMESTAMP HHMM %H%M UTC)
configure_file(version.cfg.in version.cfg @ONLY)
file(STRINGS ${CMAKE_CURRENT_BINARY_DIR}/version.cfg BASE_VERSION)

project(DataSketchesPython
        VERSION ${BASE_VERSION}
        LANGUAGES CXX)

message("Configuring DataSketches Python version ${BASE_VERSION}")

include(GNUInstallDirs)
include(CMakeDependentOption)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(CMAKE_MACOSX_RPATH ON)

set(CMAKE_CXX_STANDARD 17)

# enable compiler warnings globally
# derived from https://foonathan.net/blog/2018/10/17/cmake-warnings.html
# and https://arne-mertz.de/2018/07/cmake-properties-options/
if (MSVC)
  add_compile_options(/W4)
  set(CMAKE_DEBUG_POSTFIX "d")
else()
  add_compile_options(-Wall -pedantic -W -Wextra)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  add_compile_options(-Wimplicit-fallthrough=3)
endif()

# Code generation options, to ensure shaerd libraries work and are portable
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.18.0")
find_package(Python 3.8
  REQUIRED COMPONENTS Interpreter Development.Module NumPy
  OPTIONAL_COMPONENTS Development.SABIModule)
else()
find_package(Python COMPONENTS Interpreter Development NumPy REQUIRED)
endif()

if(Python_VERSION VERSION_LESS "3.8")
  message(FATAL_ERROR "DataSketches requires at least Python3.8")
endif()

execute_process(
  COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
  OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE NB_DIR)
list(APPEND CMAKE_PREFIX_PATH "${NB_DIR}")
find_package(nanobind CONFIG REQUIRED)

nanobind_add_module(python MODULE LTO NOMINSIZE STABLE_ABI NB_STATIC)

set_target_properties(python PROPERTIES
  PREFIX ""
  OUTPUT_NAME _datasketches
)

target_include_directories(python
  PUBLIC
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# ensure we make a .so on Mac rather than .dylib
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_target_properties(python PROPERTIES SUFFIX ".so")
endif()

target_sources(python
  PRIVATE
    src/datasketches.cpp
    src/hll_wrapper.cpp
    src/kll_wrapper.cpp
    src/cpc_wrapper.cpp
    src/fi_wrapper.cpp
    src/theta_wrapper.cpp
    src/tuple_wrapper.cpp
    src/vo_wrapper.cpp
    src/ebpps_wrapper.cpp
    src/req_wrapper.cpp
    src/quantiles_wrapper.cpp
    src/density_wrapper.cpp
    src/ks_wrapper.cpp
    src/count_wrapper.cpp
    src/vector_of_kll.cpp
    src/py_serde.cpp
)

cmake_policy(SET CMP0097 NEW)
include(ExternalProject)
ExternalProject_Add(datasketches
  GIT_REPOSITORY https://github.com/apache/datasketches-cpp.git
  GIT_TAG 5.0.2
  GIT_SHALLOW true
  GIT_SUBMODULES ""
  INSTALL_DIR /tmp/datasketches
  CMAKE_ARGS -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=/tmp/datasketches
)
ExternalProject_Get_property(datasketches INSTALL_DIR)
set(datasketches_INSTALL_DIR ${INSTALL_DIR})
message("Source dir of datasketches = ${datasketches_INSTALL_DIR}")
message("Numpy include dir(s): ${Python_NumPy_INCLUDE_DIRS}")
target_include_directories(python 
  PRIVATE
    ${datasketches_INSTALL_DIR}/include/DataSketches
    ${Python_NumPy_INCLUDE_DIRS}
    )
add_dependencies(python datasketches Python::NumPy)
