# A Comparison of Python Sketching Libraries

These notebooks aim to provide a fair comparison between the Python support for 
[Apache Software Foundation (ASF) DataSketches](https://pypi.org/project/datasketches/) and the [datasketch](https://pypi.org/project/datasketch/)
library.    
The notebooks are not an attempt to fully characterize either library's implementation, rather to highlight differences that manifest as a result 
of design choices for each library.

## Summary

As of the time of writing, November 2023, the versions under comparison are:
```
Name: datasketches
Version: 4.1.0

Name: datasketch
Version: 1.6.4
```

## 1. Distinct Counting 

### 1a. HyperLogLog
Each library implements various algorithms that fall under the HyperLogLog umbrella. 
They can be summarized as follows

|                       |   ASF                                                                                                                                      |   datasketch                                   |                                                                                                           |
|-----------------------|--------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------|-----------------------------------------------------------------------------------------------------------|
|                       |   HyperLogLog                                                                                                                              |   HyperLogLog                                  |   HyperLogLog++                                                                                           |
|   Hash functions      |   64 bit MurmurHash                                                                                                                        |   SHA1 32 bit.  Others are possible.           |   SHA1 64 bit.  Others are possible.                                                                      |
|   Bucket sizes (bits) |   4, 6, 8                                                                                                                                  |   4                                            |   8                                                                                                       |
|   Estimator           | Exact mode for small cardinalities  Historic Inverse Probability (HIP) for a single sketch; Harmonic mean after merging multiple sketches  | No exact mode.  Harmonic mean for all sketches | Harmonic mean for all sketches (single or merged) with bias corrections at small and large cardinalities  |


Since `datasketch.hyperloglog.HyperLogLog` uses only $32$ bit hash functions, we do not regard this as an appropriate solution for 
industry-applications because the error estimates will vary wildly when the input is large enough.
To ensure the fairest comparison between the sketches, we will only compare the ASF implementations with `datasketch.hyperloglog.HyperLogLogPlusPlus`
with the MurmurHash function over $64$ bits using `mmh3.hash64(x, signed=False)[0]`.