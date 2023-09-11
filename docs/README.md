Follow these steps to build the documentation.
1. Clone the directory in an appropriate location `git clone https://github.com/apache/datasketches-python.git`
2. Switch to the correct branch: `git checkout python-docs`.
3. In project root run `source python-docs-venv/bin/activate`

If there are problems running the virtual env then you may need to install `virtualenv`
and install the packages manually as below
(nb my environment has `python` aliased to `python3` so just use whichever is appropriate for your installation)
```
python -m venv python-docs-venv # create a new virtual env named python-docs-venv
source python-docs-venv/bin/activate
python -m pip install sphinx 
python -m pip install sphinx-rtd-theme
```
4. In project root run `python3 -m pip install .` to build the python bindings.
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

