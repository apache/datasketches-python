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
from math import floor, ceil
from datasketches import ebpps_sketch, PyIntsSerDe, PyStringsSerDe

class EbppsTest(unittest.TestCase):
  def test_ebpps_example(self):
    k = 50  # a small value so we can easily fill the sketch
    sk = ebpps_sketch(k)

    # ebpps sampling reduces to standard reservoir sampling
    # if the items are all equally weighted, although the
    # algorithm will be significantly slower than an optimized
    # reservoir sampler
    n = 5 * k
    for i in range(0, n):
      sk.update(i)
    
    # we should have k samples since they're equally weighted
    self.assertAlmostEqual(k, sk.c, places=12)

    # we can also add a heavy item, using a negative value to
    # be able to identify the item later.  Keep in mind that
    # "heavy" is a relative concept, so using a fixed
    # multiple of n may not be considered a heavy item for
    # larger values of n
    sk.update(-1, 1000 * n)
    self.assertEqual(k, sk.k)
    self.assertLess(sk.c, k)
    self.assertEqual(n + 1, sk.n)
    self.assertFalse(sk.is_empty())

    # we can easily get the list of items in the sample
    items = list(sk)
    self.assertLessEqual(len(items), k)

    count = 0
    for items in sk:
      count = count + 1
    self.assertTrue(count == floor(sk.c) or count == ceil(sk.c))

    # next we'll create a second, smaller sketch with
    # only heavier items relative to the previous sketch
    k2 = 5
    sk2 = ebpps_sketch(k2)
    # for weight, use the sum of all items >=0 from before (n)
    wt = n
    for i in range(0, k2 + 1):
      sk2.update((2 * n) + i, wt)

    # now merge the sketches, noting how the
    # resulting k will be the smaller of the two.
    input_sk_c = sk.c
    sk.merge(sk2)

    self.assertEqual(n + k2 + 2, sk.n)
    self.assertFalse(sk.is_empty())
    self.assertEqual(sk.k, k2)
    self.assertLess(sk.k, k)

    # the expected number of samples post-merge may be larger than
    # with the input sketch
    self.assertGreater(sk.c, input_sk_c)

    # we can dump a sumamry of information in the sketch
    print(sk)

    # if we want to print the list of items, there must be a
    # __str__() method for each item (which need not be the same
    # type; they're all generic python objects when used from
    # python), otherwise you may trigger an exception.
    # to_string() is provided as a convenience to avoid direct
    # calls to __str__() with parameters.
    print(sk.to_string(True))

    # finally, we can serialize the sketch by providing an
    # appropriate serde class.
    expected_size = sk.get_serialized_size_bytes(PyIntsSerDe())
    b = sk.serialize(PyIntsSerDe())
    self.assertEqual(expected_size, len(b))

    # if we try to deserialize with the wrong serde, things break
    try:
      ebpps_sketch.deserialize(b, PyStringsSerDe())
      self.fail()
    except:
      # expected; do nothing
      self.assertTrue(True)
      
    # using the correct serde gives us back a copy of the original
    rebuilt = ebpps_sketch.deserialize(b, PyIntsSerDe())
    self.assertEqual(sk.k, rebuilt.k)
    self.assertEqual(sk.c, rebuilt.c)
    self.assertEqual(sk.n, rebuilt.n)

    # check that printing works as expected
    self.assertGreater(len(sk.to_string(True)), 0)
    self.assertEqual(len(sk.__str__()), len(sk.to_string()))

if __name__ == '__main__':
  unittest.main()
