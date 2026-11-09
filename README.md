*This project has been created as part of the 42 curriculum by V.*

# ft_printf

> Because `ft_putnbr()` and `ft_putstr()` aren't enough.

---

## Description

**ft_printf** is a re-implementation of the C standard library function `printf()`, packaged as a static library (`libftprintf.a`). The goal of the project is to learn how **variadic functions** work in C (`<stdarg.h>`), how a format string is parsed, and how to produce output with a minimal number of system calls while handling every edge case the original function handles.

The prototype is:

```c
int ft_printf(char const *format, ...);
```

It writes to standard output and returns the **number of characters printed**, or **-1** if a write error occurs, exactly like `printf()`.

### Supported conversions

| Conversion | Output |
|------------|--------|
| `%c` | A single character (the byte `0` is printed correctly too) |
| `%s` | A string; `NULL` prints `(null)` |
| `%p` | A `void *` in hexadecimal with the `0x` prefix; `NULL` prints `(nil)` |
| `%d`, `%i` | A signed decimal integer (including `INT_MIN`) |
| `%u` | An unsigned decimal integer |
| `%x` | An unsigned hexadecimal integer, lowercase |
| `%X` | An unsigned hexadecimal integer, uppercase |
| `%%` | A literal percent sign |

> The bonus part (flags `-0.# +` and field width) is **not implemented**.

### Project rules respected

- Written in C, following the 42 **Norm**.
- Authorized functions only: `write`, `va_start`, `va_arg`, `va_end` (`malloc`, `free` and `va_copy` are allowed but not needed: the library **never allocates memory**, so it cannot leak).
- The buffer management of the original `printf()` is **not** reimplemented.
- The library is created with `ar` (no `libtool`) at the root of the repository.
- The Makefile provides `NAME`, `all`, `clean`, `fclean`, `re`, uses `cc -Wall -Wextra -Werror` and does not relink.
- `libft` is not used: the library is self-contained.

---

## Instructions

### Requirements

- `cc` (gcc or clang), `make` and `ar`
- A Unix-like system (developed and tested on Linux, x86-64)

### Build

```sh
make          # builds libftprintf.a
make clean    # removes object files
make fclean   # removes object files and libftprintf.a
make re       # fclean + all
```

### Use it in your own program

```c
/* main.c */
#include "ft_printf.h"

int main(void)
{
    int n;

    n = ft_printf("Hello %s, you are %d years old. Hex: %x, ptr: %p\n",
            "world", 42, 255, (void *)0x1234);
    ft_printf("printed %d characters, 100%%\n", n);
    return (0);
}
```

```sh
cc -Wall -Wextra -Werror main.c -L. -lftprintf -o demo
# or
cc -Wall -Wextra -Werror main.c libftprintf.a -o demo
```

### Repository layout

```
.
├── Makefile
├── ft_printf.h          # prototypes, includes and digit tables
├── ft_printf.c          # ft_printf, ft_step, ft_conv
├── ft_printf_utils.c    # ft_putchar, ft_putn, ft_putstr
├── ft_printf_nbr.c      # ft_putbase, ft_putnbr, ft_putptr
└── README.md
```

### Testing

The function is compared with the real `printf()`. A simple way is to compile the same test file twice and compare the output and the return values:

```sh
cc -DFT tests.c libftprintf.a -o t_ft      # uses ft_printf
cc tests.c -o t_std                        # uses printf
./t_ft > a.txt; ./t_std > b.txt; cmp a.txt b.txt && echo "IDENTICAL"
```

(`tests.c` defines `PF` as `ft_printf` when `-DFT` is given and as `printf` otherwise.)

Recommended edge cases: `INT_MIN`, `INT_MAX`, `UINT_MAX`, `0` with every numeric conversion, `%c` with `0`, `%s` with `NULL` and `""`, `%p` with `NULL` and `(void *)-1`, `%%`, empty format string, and several conversions in a row. Also run with `valgrind` or `-fsanitize=address,undefined`.

**Results obtained:** 60,000 randomly generated formats (text mixed with all conversions and extreme values) compared byte by byte and by return value with the real `printf()`: **0 differences**. No errors with AddressSanitizer/UBSan. Compiles without warnings using `-Wall -Wextra -Werror -pedantic`.

---

## Technical choices: algorithm and data structures

### Overall algorithm

`ft_printf` makes **a single pass** over the format string (O(n) in the length of the format plus the size of the output). At each step it consumes one *unit*:

- a **run of plain text** (everything up to the next `%` or the end of the string), or
- a **conversion** (`%` followed by one letter).

```
ft_printf ──► ft_step ──┬─► ft_putn      (plain text run)
                        └─► ft_conv ──┬─► ft_putchar   (%c)
                                      ├─► ft_putstr    (%s) ──► ft_putn
                                      ├─► ft_putptr    (%p) ──► ft_putbase
                                      ├─► ft_putnbr    (%d %i) ──► ft_putbase
                                      └─► ft_putbase   (%u %x %X)
                                                  every output ends in write()
```

Every helper returns the number of bytes written, or `-1` if `write` failed. `ft_printf` simply accumulates these values, stops at the first error and returns `-1`, otherwise the total. This makes the return value exact and error handling uniform without global variables.

### Why these choices

| Decision | Justification |
|---|---|
| **One `write` per text run and one per number** | Each `write` is a system call (user → kernel mode switch). Writing character by character is much slower. For `ft_printf("hola mundo %d\n", 42)` this means **3 calls** (11, 2 and 1 bytes) instead of 13. The subject forbids reimplementing `printf`'s buffering, so grouping naturally contiguous data is the best we can do without a buffer. |
| **A single numeric engine `ft_putbase`** | `%d %i %u %x %X %p` all reduce to "print an unsigned number in base *b*". The function receives the digit table (`"0123456789"`, `"0123456789abcdef"` or `"0123456789ABCDEF"`), the base length and a sign flag. One function, five conversions: less code, fewer bugs. |
| **A 24-byte stack buffer filled right to left** | Digits come out of `n % base` in reverse order. Filling the buffer from the end avoids reversing or recursing (recursion would also cost one `write` per digit), needs no `malloc` (no leaks, no failure path) and lets the whole number be emitted with one `write`. 24 bytes cover the worst case: 20 digits of `ULLONG_MAX` plus the sign. |
| **`unsigned long long` for the magnitude** | It holds any `unsigned int`, the absolute value of `INT_MIN` and a 64-bit pointer. |
| **`INT_MIN` handled by widening first** | `-INT_MIN` overflows `int` (undefined behaviour). `ft_putnbr` converts to `long long` *before* negating, so `-2147483648` is printed correctly. |
| **`va_arg` with the correct promoted type** | `%c` reads an `int` (a `char` is promoted to `int` through `...`). `%u/%x/%X` read `unsigned int`; reading `int` would sign-extend negative values and print `ffffffffffffffff` instead of `ffffffff`. |
| **`va_list *` passed to the helper** | The conversion helper advances the same cursor the caller sees. Passing the `va_list` by value is not guaranteed to work by the standard. |
| **No `va_copy`** | The arguments are traversed exactly once, so a copy would only add code. |
| **`if` chain dispatcher instead of a function-pointer table** | With 9 conversions the chain is shorter, easier to read and fits the Norm (no `switch` allowed). A table would require unifying function signatures for no measurable gain. |
| **`ft_step` with `char const **`** | A step must return how many bytes were written *and* advance the position in the format. Passing the address of the pointer lets one function do both. |
| **No complex data structure** | The only state is the format pointer, the `va_list` cursor (provided by the compiler) and a running total. Nothing else is needed, so nothing else is built. |

### Behaviour in undefined or unusual cases

The behaviour of glibc's `printf` was taken as reference:

- `%s` with `NULL` prints `(null)`; `%p` with `NULL` prints `(nil)` (on macOS it would be `0x0`).
- A format ending with a lone `%` returns `-1`.
- An unknown conversion such as `%y` prints `%y`.
- `ft_printf(NULL)` returns `-1` (a defensive check; it is undefined behaviour in the standard).
- A failing `write` (for example, closed stdout) stops the function, which returns `-1`.

### Known limitations

- Flags, field width and precision are not supported (bonus not implemented).
- For an invalid format like `"ab%"`, the text before the lone `%` is printed before returning `-1`; glibc prints nothing. This is undefined behaviour and outside the subject.

---

## Resources

### References

- `man 3 printf`, `man 3 stdarg`, `man 2 write` — Linux man-pages: <https://man7.org/linux/man-pages/>
- The GNU C Library Reference Manual — *Variadic Functions* and *Formatted Output*: <https://www.gnu.org/software/libc/manual/>
- ISO/IEC 9899 (C standard) — variable arguments (`<stdarg.h>`) and the `fprintf` conversion specifications.
- Kernighan & Ritchie, *The C Programming Language*, 2nd edition (chapter on variable-length argument lists).
- *System V Application Binary Interface, AMD64 Architecture Processor Supplement* — calling convention and the layout of `va_list`.
- Beej's Guide to C Programming: <https://beej.us/guide/bgc/>
- 42 Norm (official PDF).

### Use of AI:  
AI was used to write this README. The code itself was written and tested by me.

