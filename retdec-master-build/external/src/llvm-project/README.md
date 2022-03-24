# LLVM

A clone of [LLVM](https://llvm.org/) 8.0.0 modified for use in [RetDec](https://github.com/avast/retdec) and associated tools.

**Warning: LLVM in this repository was modified and does not behave the same as the vanilla version!**

## License

Copyright (c) 2003-2019 University of Illinois at Urbana-Champaign. Licensed under the University of Illinois/NCSA Open Source License. See the `LICENSE.txt` file for more details.

## Contributing

In order to improve the decompilation quality, it is sometimes needed to modify LLVM libraries used by the RetDec decompiler. Any such modification must be consulted with RetDec developers. All modifications must be delimited and marked with keyword `RetDec` so that it is possible to migrate them to new LLVM sources when the LLVM version is upgraded. When modifying LLVM source code, use the same coding conventions as LLVM uses.
