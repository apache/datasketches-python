Follow these steps to build the documentation.
1. Clone the directory in an appropriate location `git clone https://github.com/apache/datasketches-python.git`
2. Make a new branch and switch to that branch.
```
cd datasketches-python
git branch new-branch
git checkout new-branch
``` 
3. In project root, make a new virtual environment with the appropriate packages.  Depending on how python is aliased in your environment, you may 
need `python` or `python3`, as indicated by `python(3)`.
```
python -m venv venv # create a new virtual env named venv using system python
source venv/bin/activate
python(3) -m pip install sphinx # now using venv python
python(3) -m pip install sphinx-rtd-theme
```
4. In project root run `python(3) -m pip install .` to build the python bindings.
5. Build and open the documentation:
```
cd python/docs
make html
open build/html/index.html
```

## Problems
The `density_sketch` and `tuple_sketch` are not yet included.  
I have not included the file to avoid cluttering the PR with things that may not work.
You can easily include them by making a `density_sketch.rst` file in the same location as 
all of the other `X.rst` files for the sketches and copying in the following:

```
Density Sketch
--------------

.. autoclass:: datasketches.density_sketch
    :members:
    :undoc-members:

.. autoclass:: datasketches.GaussianKernel
    :members:
```
Additionally, you will need to add the below to `index.rst`
```
Density Estimation
##################

.. toctree::
   :maxdepth: 1 

   density_sketch
```

