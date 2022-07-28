# OpenHKL Documentation

The documentation for OpenHKL is generated using
[Sphinx](https://www.sphinx-doc.org/en/master/) and [Press
](https://readthedocs.org). The prerequisite Python modules can be installed
via the Python package management tool Pip:
    
```
pip install sphinx sphinx-contrib-bibtex sphinx-press-theme breathe
```

Code reference documentation is generated from the C++ source using
[Doxygen](https://www.doxygen.nl/index.html), which can be installed via any
Linux master repository or, for example, Homebrew on MacOS. The code reference
is integrated into the sphinx user documentation via
[Breathe](https://breathe.readthedocs.io/en/latest/), installed using Pip as
above. Note that an additional dependency, `graphviz` is sometimes required by
Doxygen.

The documentation can be compiled using `make docs`, and accessed from
`build/docs/sphinx/index.html`, where `build` is the build directory specified
for OpenHKL
