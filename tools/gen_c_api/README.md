# C API generator

The C API has one function per `JSRSDKManager` method, so most of its code is
generated from a method list rather than written by hand:

| Generated file | What the generator writes into it |
|---|---|
| `include/JSR-SDK/jsr_sdk.h` | the enums and one declaration per method |
| `src/jsr_sdk_c.cpp` | the enum checks and one implementation per method |
| `include/JSR-SDK/jsr_sdk.hpp` | one `jsr::Manager` method per method |
| `test/CApiParityTest.cpp` | one parity test per getter and setter |
| `test/EnumParityTest.cpp` | one test per enum: the .NET SDK has the same names and values |

Everything else in those files (conventions, structs, callbacks, struct
conversions, the test fixture) is written by hand in `templates/`.

## Adding an SDK method
1. Add it to `JSRSDKManager.hpp` and to the adapter in `src/JSRSDKManager.cpp`.
2. Add one line to `methods.py`. The docstring there explains the format.
3. Regenerate and commit the generated files together with the change:
   ```
   python tools/gen_c_api/generate.py
   ```

The generator needs Python 3 and `clang-format` (from LLVM; set `CLANG_FORMAT`
if it isn't on `PATH`). It formats its output with the repository's
`.clang-format`.

## Checks
- `python tools/gen_c_api/generate.py --check` fails when a generated file
  doesn't match what the generator produces. CI runs it, so hand edits to
  generated files and forgotten regenerations fail the build.
- `ci/check_c_api_parity.py` fails when a `JSRSDKManager` method has no C
  function.
