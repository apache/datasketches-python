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

import unittest

from datasketches import PyStringsSerDe


class SerDeTest(unittest.TestCase):
    def test_long_string_round_trip(self):
        serde = PyStringsSerDe()
        value = 'x' * (1 << 24)
        data = serde.to_bytes(value)
        decoded, consumed = serde.from_bytes(data, 0)
        self.assertEqual(consumed, len(data))
        self.assertEqual(decoded, value)

    def test_truncated_string(self):
        serde = PyStringsSerDe()
        first = serde.to_bytes('first')
        second = serde.to_bytes('second')
        for tail in (second[:3], second[:-1]):
            with self.subTest(tail=tail), self.assertRaises(IndexError):
                serde.from_bytes(first + tail, len(first))


if __name__ == '__main__':
    unittest.main()
