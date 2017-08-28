
Lya - Localize Your Application
==============

This README is for the developing version of L10ns. Please checkout the master branch to see the current stable source code.


Operating system | Status
---------------- | ----------
Ubuntu | [![TravisCI](https://img.shields.io/travis/tinganho/lya/master.svg)](https://travis-ci.org/tinganho/lya)
macOS | [![TravisCI](https://img.shields.io/travis/tinganho/lys/master.svg)](https://travis-ci.org/tinganho/lya)
Windows | [![TravisCI](https://img.shields.io/travis/tinganho/lys/master.svg)](https://travis-ci.org/tinganho/lya)

## Build

### Debug Build
Make a debug build by following commands:
```
git clone --recursive git://github.com/tinganho/l10ns.git
cd l10ns
mkdir Debug
cd Debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```
## License
Copyright (c) 2014 Tingan Ho
Licensed under the Apache 2 license.
