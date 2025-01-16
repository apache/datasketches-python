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
from datasketches import tdigest_float, tdigest_double
import numpy as np

class TdigestTest(unittest.TestCase):
    def test_tdigest_double_example(self):
      n = 2 ** 20

      # create a tdigest and inject ~1 million N(0,1) points, both using a vector
      # update as well as a single value
      td = tdigest_double()
      td.update(np.random.normal(size=n-1))
      td.update(0.0)

      # 0 should be near the median
      self.assertAlmostEqual(0.5, td.get_rank(0.0), delta=0.1)

      # the median should be near 0
      self.assertAlmostEqual(0.0, td.get_quantile(0.5), delta=0.1)

      # note that with t-digest, while it typically performs quite well in practice,
      # we do not have any sort of theoretical guarantees on the error bounds
      # or even an estimate of what bounds we may expect.

      # we also track the min/max independently from the rest of the data
      # which lets us know the full observed data range
      self.assertLessEqual(td.get_min_value(), td.get_quantile(0.01))
      self.assertLessEqual(0.0, td.get_rank(td.get_min_value()))
      self.assertGreaterEqual(td.get_max_value(), td.get_quantile(0.99))
      self.assertGreaterEqual(1.0, td.get_rank(td.get_max_value()))

      # and a few basic queries about the sketch
      self.assertFalse(td.is_empty())
      self.assertEqual(td.get_total_weight(), n)

      # we can get the PMF and CDF
      pmf = td.get_pmf([-0.5, 0.0, 0.5])
      self.assertEqual(len(pmf), 4)
      self.assertAlmostEqual(sum(pmf), 1.0)

      cdf = td.get_cdf([0.0])
      self.assertEqual(len(cdf), 2)
      self.assertAlmostEqual(cdf[0], 0.5, delta = 0.05)

      # we can define a new tdigest with a different distribution, then merge them
      td2 = tdigest_double()
      td2.update(np.random.normal(loc=2.0, size=n))
      td.merge(td2)

      # the new median should be near 1.0, and 1.0 should be near the median although
      # the error distribution is not well-characterized so we allow generous margins
      self.assertAlmostEqual(0.5, td.get_rank(1.0), delta=0.2)
      self.assertAlmostEqual(1.0, td.get_quantile(0.5), delta=0.2)
      self.assertEqual(td.get_total_weight(), 2 * n)

      # finally, can serialize and deserialize the sketch
      td_bytes = td.serialize()
      new_td = tdigest_double.deserialize(td_bytes)
      self.assertEqual(td.get_total_weight(), new_td.get_total_weight())
      self.assertEqual(td.get_min_value(), new_td.get_min_value())
      self.assertEqual(td.get_max_value(), new_td.get_max_value())
      self.assertEqual(td.get_quantile(0.7), new_td.get_quantile(0.7))
      self.assertEqual(td.get_rank(0.0), new_td.get_rank(0.0))


    # the same tests as above, but with tdigest_float
    def test_tdigest_float_example(self):
      n = 2 ** 20
      td = tdigest_float()
      td.update(np.random.normal(size=n-1))
      td.update(0.0)

      self.assertAlmostEqual(0.5, td.get_rank(0.0), delta=0.1)
      self.assertAlmostEqual(0.0, td.get_quantile(0.5), delta=0.1)

      self.assertLessEqual(td.get_min_value(), td.get_quantile(0.01))
      self.assertLessEqual(0.0, td.get_rank(td.get_min_value()))
      self.assertGreaterEqual(td.get_max_value(), td.get_quantile(0.99))
      self.assertGreaterEqual(1.0, td.get_rank(td.get_max_value()))

      self.assertFalse(td.is_empty())
      self.assertEqual(td.get_total_weight(), n)

      pmf = td.get_pmf([-0.5, 0.0, 0.5])
      self.assertEqual(len(pmf), 4)
      self.assertAlmostEqual(sum(pmf), 1.0)

      cdf = td.get_cdf([0.0])
      self.assertEqual(len(cdf), 2)
      self.assertAlmostEqual(cdf[0], 0.5, delta = 0.05)

      td2 = tdigest_float()
      td2.update(np.random.normal(loc=2.0, size=n))
      td.merge(td2)

      self.assertAlmostEqual(0.5, td.get_rank(1.0), delta=0.2)
      self.assertAlmostEqual(1.0, td.get_quantile(0.5), delta=0.2)
      self.assertEqual(td.get_total_weight(), 2 * n)

      td_bytes = td.serialize()
      new_td = tdigest_float.deserialize(td_bytes)
      self.assertEqual(td.get_total_weight(), new_td.get_total_weight())
      self.assertEqual(td.get_min_value(), new_td.get_min_value())
      self.assertEqual(td.get_max_value(), new_td.get_max_value())
      self.assertEqual(td.get_quantile(0.7), new_td.get_quantile(0.7))
      self.assertEqual(td.get_rank(0.0), new_td.get_rank(0.0))
