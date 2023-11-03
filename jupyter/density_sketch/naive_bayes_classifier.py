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
from sklearn.naive_bayes import GaussianNB
from sklearn.metrics import accuracy_score
from scipy.stats import norm
from sklearn.neighbors import KernelDensity
import matplotlib.pyplot as plt
from kernel_density import kernel_density
from datasketches import density_sketch

class NaiveBayes:
    def __init__(self, kernel='gaussian', bandwidth=1.0, coreset=False, coreset_k=None, coreset_dim=None):
        self.kernel = kernel
        self.bandwidth = bandwidth
        self.class_priors = None
        self.class_kdes = None
        self.using_coreset = coreset
        if self.using_coreset:
            self.class_coresets = {}
            self.k = coreset_k
            self.d = coreset_dim

    def fit(self, X, y):
        self.X = X
        self.y = y
        self.class_priors = {}
        self.class_kdes = {}

        for c in np.unique(y):
            X_c = X[y == c]
            self.class_priors[c] = len(X_c) / len(X)
            self.class_kdes[c] = []

            if self.using_coreset:
                self.class_coresets[c] = []
                coreset = density_sketch(self.k, 1)
                for i in range(X.shape[1]):
                    # perform 1d density estimation over every feature 
                    for sample in X_c[:, i]:
                        coreset.update([sample / self.bandwidth] )
                    self.class_coresets[c].append(coreset)

    def predict_proba(self, X):
        posteriors = []
        for i in range(X.shape[0]):
            likelihoods = []
            for c in self.class_priors:
                likelihood = self.class_priors[c]
                if self.using_coreset:
                    kde = 1.
                    for j in range(X.shape[1]):
                        kde *= self.class_coresets[c][j].get_estimate([X[i, j] / self.bandwidth])
                    #print(kde.shape)
                else:
                    kde = kernel_density(self.X[self.y == c], X[i, :].reshape(1,-1), self.bandwidth)[0]
                likelihoods.append(likelihood*kde)
            posterior = likelihoods / np.sum(likelihoods)
            posteriors.append(posterior)
        return np.array(posteriors)

    def predict(self, X):
        posteriors = self.predict_proba(X)
        return np.argmax(posteriors, axis=1)


def main():
    np.random.seed(42)

    # Generate some random data
    n_sample = 200
    X = np.concatenate([np.random.normal(0, 1, size=(n_sample, 2)), np.random.normal(2, 1, size=(n_sample, 2))], axis=0)
    y = np.concatenate([np.zeros(n_sample), np.ones(n_sample)], axis=0)

    # Split the data into training and testing sets
    indices = np.random.permutation(X.shape[0])
    train_indices, test_indices = indices[:int(0.8 * X.shape[0])], indices[int(0.8 * X.shape[0]):]
    X_train, y_train = X[train_indices], y[train_indices]
    X_test, y_test = X[test_indices], y[test_indices]

    # Train and test the Naive Bayes classifier
    nb = NaiveBayes()
    nb.fit(X_train, y_train)
    y_pred = nb.predict(X_test)
    print(f"Custom Naive Bayes Accuracy: {accuracy_score(y_test, y_pred)}")

    # Train and test the Naive Bayes classifier using a coreset
    nbc = NaiveBayes(coreset=True, coreset_k=8, coreset_dim=X.shape[1])
    nbc.fit(X_train, y_train)
    y_pred = nbc.predict(X_test)
    print(f"Coreset Naive Bayes Accuracy: {accuracy_score(y_test, y_pred)}")

    # Train and test the Sklearn Naive Bayes classifier
    gnb = GaussianNB()
    gnb.fit(X_train, y_train)
    y_pred = gnb.predict(X_test)
    print(f"Sklearn Naive Bayes Accuracy: {accuracy_score(y_test, y_pred)}")

    fig, ax = plt.subplots()
    fig.suptitle("Coreset Naive Bayes")
    buffer = 0.5
    x = np.linspace(X_train[:, 0].min() - buffer, X_train[:, 0].max() + buffer, 200)
    y = np.linspace(X_train[:, 1].min() - buffer, X_train[:, 1].max() + buffer, 200)
    X_plot, Y_plot = np.meshgrid(x, y)
    X_plot_2d = np.c_[X_plot.ravel(), Y_plot.ravel()]
    plot_preds = nbc.predict(X_plot_2d)
    plot_preds.reshape(X_plot.shape)
    ax.scatter(X_plot, Y_plot, c=plot_preds, alpha=0.01)#0875)

    # plot the markers on top of the color background
    ax.scatter(X_train[:, 0], X_train[:, 1], c=y_train, marker='d', alpha=1., edgecolor="black", label='Training data')
    ax.scatter(X_test[:, 0], X_test[:, 1], c="white", edgecolor="black", marker='o', label='Test data')
    x_star = np.array([5, -2])[np.newaxis, :]
    print("Predicting: ", x_star)
    print("Predicted class: ", nbc.predict(x_star))
    ax.legend()
    plt.show()

if __name__ == "__main__":
    main()