# A dynamic string management header-only C library

## Very much work in progress still!

Makes handling dynamic strings in C (at least somewhat) easy.
Also includes **small string optimization**.

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