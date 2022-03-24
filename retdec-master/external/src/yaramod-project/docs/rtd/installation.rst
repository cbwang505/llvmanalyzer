============
Installation
============

Requirements
============

In order to install ``yaramod`` you will need:

* `CMake <https://cmake.org/>`_ 3.6+
* C++ compiler with C++17 support

If you want to use Python bindings then you will also need:

* Python 3.6+

Compilation
===========

Yaramod is primarily written in C++ so you'll have to compile it to the library which you can then link to your own project. We do not provide any prebuild binary releases so if you
would like to use it from C++ code, you need to compile it yourself. We use `CMake <https://cmake.org/>`_ for our build system so you'll need to install it beforehand. We support
3 main platforms - Linux, Windows and macOS. The project might work also on other platforms but we do not provide any support for those nor any guarantees that it works.

In order to configure the project and start the compilation run

.. code-block:: bash

  mkdir build
  cmake -DCMAKE_BUILD_TYPE=<Release|Debug> ..
  cmake --build .

If you would also like to build examples (located in `src/examples/cpp <https://github.com/avast/yaramod/tree/master/src/examples/cpp>`_) then also pass ``-DYARAMOD_EXAMPLES=ON`` to the
second command, right after ``-DCMAKE_BUILD_TYPE=...``.

It is also possible to build Python bindings directly from CMake by providing an option ``-DYARAMOD_PYTHON=ON``.

Python Bindings
===============

Yaramod also comes with Python 3 bindings in case you don't want to deal with linking C++ libraries or you just simply prefer Python over C++. We deploy yaramod to PyPI so you can
install it directly from there using ``pip``.

.. code-block:: bash

    pip install yaramod

In case you don't want to use the latest release but would much rather use ``master`` branch you can run

.. code-block:: bash

    pip install .

from the root directory of yaramod directly.
