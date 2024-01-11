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
