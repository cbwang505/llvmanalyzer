# yaramod

[![Travis CI build status](https://travis-ci.org/avast/yaramod.svg?branch=master)](https://travis-ci.org/avast/yaramod)
[![AppVeyor build status](https://ci.appveyor.com/api/projects/status/github/avast/yaramod?branch=master&svg=true)](https://ci.appveyor.com/project/avast/yaramod?branch=master)
[![Documentation Status](https://readthedocs.org/projects/yaramod/badge/?version=latest)](https://yaramod.readthedocs.io/en/latest/?badge=latest)

`yaramod` is a library that provides parsing of [YARA](https://github.com/VirusTotal/yara) rules into AST and a C++ programming interface to build new YARA rulesets. This project is not associated with the YARA project.

`yaramod` also comes with Python bindings and this repository should be fully compatible with installation using `pip`.

## User Documentation

You can find our documentation on [Read the Docs](https://yaramod.readthedocs.io/en/latest/).

## API Documentation

You can generate the API documentation by yourself. Pass `-DYARAMOD_DOCS=ON` to `cmake` and run `make doc`.

## License

Copyright (c) 2017 Avast Software, licensed under the MIT license. See the `LICENSE` file for more details.

`yaramod` uses third-party libraries or other resources listed, along with their licenses, in the `LICENSE-THIRD-PARTY` file.

## Contributing

See [RetDec contribution guidelines](https://github.com/avast/retdec/wiki/Contribution-Guidelines).
