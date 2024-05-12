# At Same Time

A c library for async operation

## Table of Contents

- [Install](#install)
- [Usage](#usage)
- [Example](#example)
- [Maintainers](#maintainers)
- [Contributing](#contributing)
- [License](#license)

## Install

compile and run the test program:
```cmd
make run

```
or

```cmd
gcc -o main.c main.exe -I src/include
```

the library is in the folder [src/include/at_same_time](src/include/at_same_time)
and require my other library [easy_c_data_structure](https://github.com/raiseFlaymeException/easy-c-data-structure) located in the folder [src/include/easy_c_data_structure](src/include/easy_c_data_structure)

## Usage

I try to comment stuff in the [example](#example) and in the library to make it easier

```c

#define AT_SAME_TIME_IMPL // include the implementation (see stb style library:
                          // https://github.com/nothings/stb)
#include "at_same_time/at_same_time.h"

int main(int argc, char *argv[]) {
    // init the library (will allocate memory using malloc)
    AstAsync *async = AstAsync_init(CAPACITY_FUNCTION_ARRAY, SLEEP_TIME);

    // register the function to be run asynchronously
    AstPromise promise1 = AstAsync_register(async, func, &args1);

    // get the result and wait infinitly
    size_t *result1 = AstAsync_get_result(async, promise1, NULL);

    // or 

    // get the result and wait around 500 microseconds
    // useconds_t timeout = 500; 
    // size_t *result1 = AstAsync_get_result(async, promise1, &timeout);

    // check if the function finished
    printf("result1 finished: %s\n", (AstAsync_is_func_finished(async, promise1)) ? "true" : "false");

    // deallocate memory and join every threads
    AstAsync_quit(async);
    return 0;
}

```

## Example

A better example in the file [main.c](main.c)

## Maintainers

[@raiseFlaymeException](https://github.com/raiseFlaymeException).

## Contributing

Feel free to contibute [Open an issue](https://github.com/raiseFlaymeException/at_same_time/issues/new) or submit PRs.

## License

[ZLIB](LICENSE) © raiseFlaymeException
