# A dynamic string management header-only C library

## Very much work in progress still!

Makes handling dynamic strings in C (at least somewhat) easy.
Also includes **small string optimization** and should be relatively easily detachable from the standard library.

For using the library:
```
#define STRUT_IMPLEMENTATION
#include "strut.h"
```

For building the project (includes tests):
```
mkdir build
cd build
cmake ..
cmake --build .
./strut
```
