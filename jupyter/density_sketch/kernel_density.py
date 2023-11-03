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
from scipy.spatial.distance import cdist
from scipy.stats import multivariate_normal
from sklearn.neighbors import KernelDensity
from sklearn.datasets import make_blobs

def kernel_density(Xtrain, Xtest, bandwidth=1.):
    """
    Returns the kernel density estimate between Xtrain and Xtest.
    returns:
        (1/n)*(1/bandwidth*sqrt(2pi))^d*sum_{i=1}^n K( (x* - x_i) / bandwidth )
    The bandwidth in scipy is in the numerator so we use 1./bandwidth
    The mean function picks up the 1/n factor.
    """
    for x in [Xtrain, Xtest]:
        if x.ndim == 1:
            x.reshape(-1, 1)
    g = (1./bandwidth)**2
    K = np.exp(-cdist(Xtrain, Xtest, metric='sqeuclidean')*g/2)
    K *= 1./(bandwidth*np.sqrt(2*np.pi))**Xtrain.shape[1]
    return np.mean(K, axis=0)

def test_kernel_density():
    # Generate random data
    np.random.seed(0)
    X, _ = make_blobs(n_samples=1000, centers=3, n_features=2, random_state=0)

    # Calculate true densities using scipy's multivariate_normal
    true_densities = multivariate_normal.pdf(X, mean=X.mean(axis=0), cov=X.var(axis=0))

    # Calculate estimated densities using your function
    estimated_densities = kernel_density(X, X, bandwidth=1.)

    # Check that the estimated densities are close to the true densities
    assert np.allclose(estimated_densities, true_densities, atol=0.05)

    # Check that your function gives the same results as sklearn's KernelDensity
    kde = KernelDensity(bandwidth=1., kernel='gaussian').fit(X)
    estimated_densities_sklearn = np.exp(kde.score_samples(X))
    assert np.allclose(estimated_densities, estimated_densities_sklearn, atol=0.05)


if __name__ == "__main__":
    test_kernel_density()
