#!/usr/bin/env python

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

import sys
import argparse
from datetime import datetime
import pandas as pd
import numpy as np
from utils import distinct_number_sequence
import datasketches as ds
import datasketch as d
import mmh3
import os
import matplotlib.pyplot as plt

class ErrorCardinalityProfile:
    """Generates an experiment comparing the error over different cardinalities"""
    def __init__(self, sketch_lgk:int, lg_trials:int, max_lgN:int):
        self.sketch_lgk = sketch_lgk
        self.num_trials = 2**lg_trials
        self.max_lgN = max_lgN
        self.max_num_distincts = np.uint64(2 ** self.max_lgN)
        self.directory_name = "hll_accuracy_profile_" + datetime.today().strftime('%Y%m%d')
        if not os.path.exists(self.directory_name):
            os.mkdir(self.directory_name)
        self.file_extension = "_" + datetime.today().strftime('%H%M') + f"lgK_{self.sketch_lgk}_lgT_{lg_trials}"

        # Need to remove repeated items for the program logic in self.run()
        self.plot_points = self._generate_plot_points()
        self.plot_points.extend(self._generate_plot_points())
        self.plot_points = list(set(self.plot_points))
        self.plot_points.sort()
        print(self.plot_points)

        # Initialise the data structures for results
        self.DataSketches_results_arr = np.zeros((len(self.plot_points), self.num_trials), dtype=float)
        self.datasketch_results_arr = np.zeros_like(self.DataSketches_results_arr)
        self.DataSketches_results_df = pd.DataFrame(index=self.plot_points, columns=None)
        self.datasketch_results_df = pd.DataFrame(index=self.plot_points, columns=None)
        self.data = np.random.randn(len(self.plot_points), self.num_trials)
        print("Data shape: ", self.data.shape)

    def _generate_plot_points(self) -> list:
        """
        Generates the standard sequence defining the input cardinalites for the experiment
        This is just two points at each power of 2
        """
        all_plot_points = []
        for lgk in range(1, self.max_lgN+1):
            points = np.unique(np.logspace(start=lgk, stop=lgk+1, num=8, endpoint=False, base=2, dtype=np.uint64))
            all_plot_points.extend(points)
        all_plot_points.sort()
        return all_plot_points

    def _bad_hll_range(self) -> list:
        """Generates 16 logspaced points in the n ≈ 2.5k range."""
        log2_sketch_threshold = np.log2(2.5* (2**self.sketch_lgk))
        start = np.floor(log2_sketch_threshold).astype(np.uint64)
        stop = np.ceil(log2_sketch_threshold).astype(np.uint64)+1
        points = np.logspace(start, stop, num=50, endpoint=False, base=2, dtype=np.uint64)[1:]
        return points

    def _is_power_of_two(self, a):
        """Bitwise operations to check value a is a power of two"""
        return (a & (a-1) == 0) and a != 0

    def _results_to_df(self, start_:int, end_:int, arr:np.array, df:pd.DataFrame):
        """Concatenates the array between columns start_,...end_ - 1 to the dataframe"""
        new_df = pd.DataFrame(arr[:, start_:end_], index=df.index, columns=np.arange(start_, end_).tolist())
        print("concatenating: ", new_df)
        concat_df = pd.concat([df, new_df], axis=1)
        return concat_df

    def run(self):
        """Runs the experiment"""
        seq_start = np.uint64(2345234)
        distinct_number = np.uint64(3462)
        previous_log_trial_index = 0
        ds_all_results = np.zeros((self.num_trials, len(self.plot_points)))
        d_all_results = np.zeros_like(ds_all_results)

        for trial in range(1, self.num_trials+1):
            #print(f"Trial = {trial}\t{self._is_power_of_two(trial)}")
            # Initialise the sketches
            hll = ds.hll_sketch(self.sketch_lgk, ds.HLL_8)
            h = d.HyperLogLogPlusPlus(p=self.sketch_lgk, hashfunc=lambda x: mmh3.hash64(x, signed=False)[0])
            plot_point_index = 0  # Return to the start of the plot points list to generate the data
            plot_point_value = self.plot_points[plot_point_index]
            total_updates = 0
            seq_start += distinct_number  # Start a new input sequence

            # Temporary result data structure
            ds_results = np.zeros((len(self.plot_points),))
            d_results = np.zeros_like(ds_results)


            for new_number in distinct_number_sequence(seq_start):
                hll.update(new_number)
                h.update(str(new_number).encode('utf8'))
                total_updates += 1
                #print(f"Trial: {trial:<5} updates: {total_updates:<5}Index: {plot_point_index:<5} Value: {plot_point_value:<5}\n")
                if total_updates == plot_point_value:
                    ds_results[plot_point_index] = (hll.get_estimate() - plot_point_value) / plot_point_value
                    d_results[plot_point_index] = (h.count() - plot_point_value) / plot_point_value
                    plot_point_index += 1
                    #print(f"PPI:{plot_point_index:<3}PPV:{plot_point_value}")

                    if plot_point_index < len(self.plot_points):
                        plot_point_value = self.plot_points[plot_point_index]
                    else:
                        #print("Already at the end")
                        break

            # After the break statement, control returns here.  Now need to decide whether to write or continue.
            ds_all_results[trial-1, :] = ds_results # subtract 1 as we use 1-based indexing for the trial count.
            d_all_results[trial - 1, :] = d_results  # subtract 1 as we use 1-based indexing for the trial count.
            if self._is_power_of_two(trial) and trial > 1:
                # write the array only a logarithmic number of times
                temporary_ds_results = ds_all_results[0:trial, : ]
                temporary_d_results = d_all_results[0:trial, :]
                print(f"#################### PARTIAL RESULTS FOR {trial} TRIALS: DATASKETCHES ####################")
                previous_log_trial_index = trial
                self.DataSketches_results_df = pd.DataFrame(temporary_ds_results.T, index=self.DataSketches_results_df.index, columns=np.arange(trial).tolist())
                self.DataSketches_results_df.to_csv(
                    self.directory_name + "/DataSketches_hll" + self.file_extension + f"trials_{trial}.csv",
                    index_label="n")
                self.datasketch_results_df = pd.DataFrame(temporary_d_results.T,
                                                            index=self.datasketch_results_df.index,
                                                            columns=np.arange(trial).tolist())
                self.datasketch_results_df.to_csv(
                    self.directory_name + "/datasketch_hll" + self.file_extension + f"trials_{trial}.csv",
                    index_label="n"
                )
                print(self.DataSketches_results_df)

                print(f"#################### PARTIAL RESULTS FOR {trial} TRIALS: datasketch ####################")
                print(self.datasketch_results_df)


def main(argv):
    assert len(argv) == 3
    SKETCH_LGK = argv['lgk']
    LG_TRIALS = argv['lgt']
    MAX_LG_N = argv['lgN']  # FOR SETTING NUMBER OF DISTINCTS
    experiment = ErrorCardinalityProfile(SKETCH_LGK, LG_TRIALS, MAX_LG_N)
    experiment.run()



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Error-cardinality profile of HyperLogLog')
    parser.add_argument('-lgk', '--lgk', help='Log2(k) value for the number of buckets in the sketch.',type=int, required=True)
    parser.add_argument('-lgt', '--lgt', help='Log2(T) value for number of trials t.',type=int, required=True)
    parser.add_argument('-lgN', '--lgN', help='Largest permissible log2(.) value for input size.',type=int, required=True)
    args = vars(parser.parse_args())
    main(args)
