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

import numpy as np

def distinct_number_sequence(start:np.uint64=0) -> np.uint64:
    """Generator function to make 64-bit numbers that are distinct."""
    assert isinstance(start, np.uint64)
    num = start
    golden_ratio = np.uint64(0x9e3779b97f4a7c13)
    while True:
        yield num
        num += np.uint64(golden_ratio)

if __name__ == '__main__':
    start = np.uint64(2345234635)
    ndistincts = np.uint64(10)
    count = 0
    for i in distinct_number_sequence(start):
        print(i, end="\n")
        count += 1
        if count == ndistincts:
            break
