The Future Gadget Laboratory Library
---

**libFGL** — Intended for use with Tempus Edax Rerum projects.

This is a modern C++ header-only library that dreams of growing up to be refactored into a module. A miscellaneous assortment of in-house utilities intended to replace frequently duplicated flimsy spoons with sufficiently engineered shovels.

Breaking changes are a feature. No support. No guarentees. Not for third-party production use.

An important note about documentation
---

The documentation found here is a high-level overview of libFGL's components which covers the rationale, explanations, examples, and a brief description of interfaces. **The most accurate and up-to-date *reference material* will always be the code and comments in the headers themselves.** libFGL's headers are written to be read and have meaningful comments and names: the interfaces and abstractions are usually concise and relatively self-explanatory. The source code for small example programs are also included.

Only build environments using C++20 & GCC 12 or higher are officially compatible, but most other compilers are partially compatible.

See the [libFGL compatibility check](#libfgl-compatibility-check) documentation for more information.

# Table of Contents

<!-- @import "[TOC]" {cmd="toc" depthFrom=1 depthTo=6 orderedList=false} -->

<!-- code_chunk_output -->

- [Table of Contents](#table-of-contents)
- [Getting Started](#getting-started)
  - [Installing and using libFGL](#installing-and-using-libfgl)
  - [Compiling and running tests](#compiling-and-running-tests)
  - [Extensions for libFGL](#extensions-for-libfgl)
  - [General information](#general-information)
    - [Include folder structure](#include-folder-structure)
      - [Include everything](#include-everything)
      - [Include only what you want](#include-only-what-you-want)
    - [Internal and experimental features](#internal-and-experimental-features)
    - [Preprocessor macros](#preprocessor-macros)
      - [Short macros](#short-macros)
- [Contributing](#contributing)
  - [How to contribute](#how-to-contribute)
  - [Writing tests](#writing-tests)
  - [Test build system & test dependencies](#test-build-system-test-dependencies)
- [Documentation](#documentation)
  - [Environment](#environment)
    - [Build type](#build-type)
    - [libFGL compatibility check](#libfgl-compatibility-check)
  - [Debug](#debug)
    - [Constexpr assert](#constexpr-assert)
    - [Echo](#echo)
    - [Exception Occurs](#exception-occurs)
    - [Fixme](#fixme)
    - [Output](#output)
    - [Stopwatch](#stopwatch)
  - [I/O](#io)
    - [Binary files](#binary-files)
  - [Types](#types)
    - [C byte pointer wrapper](#c-byte-pointer-wrapper)
    - [C types](#c-types)
    - [Range alias](#range-alias)
    - [Range constraints](#range-constraints)
    - [Singleton](#singleton)
    - [Traits](#traits)
  - [Utility](#utility)
    - [Bitwise](#bitwise)
    - [Circular iterator](#circular-iterator)
    - [Enumerate](#enumerate)
    - [Linear matrix alias](#linear-matrix-alias)
      - [Linear matrix alias - Utilities](#linear-matrix-alias-utilities)
      - [Linear matrix alias - Static alias](#linear-matrix-alias-static-alias)
      - [Linear matrix alias - Dynamic alias](#linear-matrix-alias-dynamic-alias)
    - [Make byte array](#make-byte-array)
    - [Matrix](#matrix)
    - [Random](#random)
    - [Sleep](#sleep)
    - [Zip](#zip)

<!-- /code_chunk_output -->

---
# Getting Started

## Installing and using libFGL
1. Download the libFGL source to where ever you'd like.
2. Add the library include path when you compile.
	- For [clang](https://clang.llvm.org/docs/ClangCommandLineReference.html#include-path-management) and [gcc](https://gcc.gnu.org/onlinedocs/gcc/Directory-Options.html), pass `-I"your_path_to/libFGL/include"` when compiling.
	- For [MSVC](https://docs.microsoft.com/en-us/cpp/preprocessor/hash-include-directive-c-cpp?view=msvc-170), pass `/I"your_path_to/libFGL/include"` when compiling.

	*Of course, replace `your_path_to` with the path to where you installed libFGL.*

3. In your source file, include the components you want using `<>` syntax (see [Include Folder Structure](#include-folder-structure))
	```cpp
	// main.cpp
	#include <array>

	#include <FGLM> // all of libFGL + opt-in features

	/* or you can be more selective:
	#define FGL_SHORT_MACROS
	#include <fgl/debug/constexpr_assert.hpp> // constexpr_assert
	#include <fgl/types/range_constraints.hpp> // sized_range_same_as
	#include <fgl/utility/zip.hpp> // czip
	//*/

	// silly example code using fgl::zip
	constexpr int sum_int_pairs(
		const fgl::sized_range_same_as<int>& a,
		const fgl::sized_range_same_as<int>& b)
	{
		int sum{ 0 };
		for (const auto& [av, bv] : fgl::czip(a, b))
			sum += av + bv;
		return sum;
	}

	int main()
	{
		[]() consteval
		{
			constexpr std::array a{ 1, 1, 1 }; // 3 elements
			std::array b{ 1, 1, 1, 1 }; // 4 elements, last will be ignored
			constexpr_assert(sum_int_pairs(a, b) == 6);
		}();
	}
	```
4. You're done.

Other than for testing, libFGL doesn't support any particular build systems, package systems, or IDEs. You're on your own.

libFGL extensions will often require more elaborate dependencies and compilation steps. Refer to the extension's documentation.

## Compiling and running tests

libFGL is header-only, so it doesn't require compilation or linking. Just include the components you want and you're done. However, you should probably compile and run the unit tests to make sure everything's behaving properly in your environment.

Remove the `.DEFAULT` suffix from `tup.config.DEFAULT`. This file contains system-specific paths and user preferences.

Use [the `tup` build-system](https://gittup.org/gittup/) by executing `tup` in the directory that contains `Tupfile.ini`. The top-level `Tupfile` is configured to build and run all tests. If `tup` completes without warnings or errors, then the library should be fully-functional on the platform in question.

---
## Extensions for libFGL

- (WIP) **libFGL-vulkan** — a simple and modern interface for using Vulkan compute shaders.
	- Work In Progress -> [KJNeko's LearningVulkan repository](https://github.com/KJNeko/LearningVulkan)

---
## General information

---
### Include folder structure

#### Include everything
The `include/fgl.hpp` includes all top-level `include/fgl/*.hpp` headers which should include *almost* the entire library; everything except experimental and opt-in features. `include/FGLM` includes `include/fgl.hpp` but also all opt-in features like [short macro by defining `FGL_SHORT_MACROS`](#short-macros) and exposes `using namespace fgl::literals;`. libFGL extensions must be included separately.

#### Include only what you want
Each include folder has a corresponding `.hpp` file of the same name which includes all header files in that folder. For example `FOLDER.hpp` will include all `FOLDER/*.hpp` files. `<fgl.hpp>` includes all of `fgl/*.hpp`, and `<fgl/debug.hpp>` includes all of `fgl/debug/*.hpp`, and so on.

---
### Internal and experimental features
Anything (folder, file, namespace, preprocessor definition, etc.) containing one or more of these words is internal to the FGL library. You should never have to explicitly include these files or use these components directly.
- `experimental`
- `WIP` / `work_in_progress`
- `unfinished`
- `internal` / `details`
- `test`
- any name prefixed with an underscore

Interal preprocessor definitions will always begin with `FGL_INTERNAL`.

Experimental components shouldn't be used and are things that may or may not become part of libFGL at some point. Some experimental headers are in active developement are used for rapid evolution and discovery; while others are old copy+paste code snippets from other projects (including personal projects) which may be used as idea-fodder or are to be rewritten as libFGL components.

---
### Preprocessor macros

While libFGL usually tries to avoid preprocessor macros, there are still some features that are best implemented by utilizing the preprocessor; mainly for purposes of code generation, expression stringification, and meaningful compiler diagnostics.

libFGL promises to not redefine macros. An `#error` will occur if a symbol already exists.

#### Short macros

To avoid name collisions, libFGL macros come in two flavors: the default "long" macros (ex: `FGL_DEBUG_CONSTEXPR_ASSERT` and `FGL_DEBUG_FIX_ME`) and opt-in "short" macros (ex: `constexpr_assert` and `FIX_ME`). Headers that define non-internal preprocessor macros will normally specify a `#define` switch for enabling short macros for the header in question by defining a symbol before header inclusion; `FGL_DEBUG_FIXME_SHORT_MACROS` for example. Defining the "global" `FGL_SHORT_MACROS` will enable / opt-in to all short macros. Using the non-global switches lets you be more selective about which short macros you opt-in to.

---
# Contributing

---
## How to contribute

Follow these easy steps:

1. Don't.

Just open an issue to report a bug, request a feature, or just to say hi if you're as lonely as us.

---
## Writing tests

Each component of the library, where applicable, will have a unit test in the `test/` directory with a folder name that matches the include structure with separators replaced with underscores. For example, `include/fgl/debug/fixme.hpp` corresponds with `test/fgl_debug_fixme`.

Tests should have minimal includes and namespace polution. Test programs return `EXIT_SUCCESS` to signal that the test(s) have passed. Tests should strive to use compile-time `static_assert` and `consteval` functions where ever possible. Where not possible, use `assert`. Where not possible, a test failure should cause the program to return code `EXIT_FAILURE`. Tests should not use standard input/out (i.e. `cout`, `cin`). Each test (i.e. assertion) should be granular and represent a minimal subset of component behavior in order to improve debugability. Don't accumulate failures where possible; prefer to fail hard and fast. Use descriptive variable and function names. The intent and conditions of a test should be self-evident in code or at the very least clearly documented by comments.

---
## Test build system & test dependencies

Each test (`test/fgl_component/`) will contain a `Tupfile` which defines rules for compilation, linking, and running of the test. Optionally, a test can have a `Tuprules.tup` file which can be used to determine *test ordering* (prerequisites / dependencies) based on folder name.

***An important limitation of this is that the required test names must exist and be correct. If a required test doesn't exist, the requirement is silently ignored. That would be bad.*** They're more like *guidelines* than actual rules.


**Walkthrough and example**

**`Tupfile`**
Tup files look confusing but are quite simple. They work based on `:` rules which define some recipe like `inputs |> shell command |> outputs`. The inputs and outputs of the `:` rules are used by tup to create a directed acyclic graph which describes the build order and file-based dependencies.

Tests are referred to by the name of the folder that contains the test's `Tupfile`, which is the "current working directory" for that test. (`%d` in rules, from here on `[CWD]`)

```sh
### Unmodified. If you modify this, remove this line and document your changes.
include_rules
: foreach src/*.cpp | $(TEST_PREREQUISITE) |> !C |> $(TEST_OBJ_DIR)/%d/%B.o {test_objs}
: {test_objs} |> !L |> $(TEST_BIN_DIR)/%d/%d.exe {unit_test}
: {unit_test} |> !RUN_TEST |> $(TEST_DIR)/<%d>
: | $(TEST_DIR)/<%d> |> !PASSTHROUGH |> <unit_test_results>
```

Note: `$()` macros like `TEST_OBJ_DIR` are defined in a higher-level `Tuprules.tup`.

0. `include_rule` includes all `Tuprules.tup` files from the CWD and above.

1. The first `:` rule requires `$(TEST_PREREQUISITES)` which is optionally defined in the same-level `Tuprules.tup`. Assuming the requirements are met, it compiles each `.cpp` file to an `.o` file of the same name and output to `$(TEST_OBJ_DIR)/[CWD]/` which is then grouped into a local tup "bin" specific to the test, named `{test_objs}`.

2. The second `:` rule links all of the object files referred to by `{test_objs}` to produce a `$(TEST_BIN_DIR)/[CWD]/[CWD].exe`, which is grouped into a local tup bin named `{unit_test}`.

3. The third `:` rule executes the unit test program. If the test passes (returns `EXIT_SUCCESS`) it's added to the tup group `$(TEST_DIR)/<[CWD]>`, so that other tests may refer to this one by it's directory.

4. The fourth `:` rule forwards the `$(TEST_DIR)/<[CWD]>` test result to a global `<unit_test_results>`group. This is the group which is required by the top-level project `Tupfile` (simply `: | <unit_test_results> |> |>`, a rule which requires all test results as input).

Some tests require specialized rules, like passing arguments to the test program. In such cases, like `fgl_io_binary_files`, the `### Unmodified` comment is removed and the changes are documented.

**`Tuprules.tup`**
Using `test/fgl_utility_enumerate` as an example:
```sh
TEST_PREREQUISITE= $(TEST_DIR)/<fgl_utility_zip> $(TEST_DIR)/<fgl_types_traits>
```
Here, `fgl_utility_enumerate` requires that the `fgl_utility_zip` and `fgl_types_traits` tests pass before `fgl_utility_enumerate`'s test is run.


---
# Documentation

---
## Environment

---
### Build type

`#include <fgl/environment/build_type.hpp>`

`constexpr` boolean values represent the build environment of the translation unit by definition detection of the macro `NDEBUG`. If `NDEBUG` is not defined, the environment is assumed to be in a debug state.

**Important limitations**

Include order and location matters.

The booleans are only a valid reflection of the program state at the point of inclusion because the `NDEBUG` symbol may be defined, un-defined, or re-defined at any point before or after inclusion of this header which may make the boolean state invalid (not an accurate reflection of the the macro).

For this reason, usage of `#ifdef` and `#ifndef` is recommended in translation units which may `#define` and `#undef` the `NDEBUG` symbol frequently or with unpredictable header ordering.

**Interface**

1. `false` if `NDEBUG` was defined at time of including the header, otherwise `true`

	```cpp
	[[maybe_unused]] inline constexpr bool debug_build
	```

2. The opposite (boolean negation) of `debug_build`

	```cpp
	[[maybe_unused]] inline constexpr bool release_build
	```

---
### libFGL compatibility check
`#include <fgl/environment/libfgl_compatibility_check.hpp>`

This header is included by nearly everything in libFGL. It performs preprocessor checks to ensure an *officially compatible* environment (minimum C++ language specification and compiler version).

**The constraints on official compatibility are extremely narrow.** The true viability of an environment will vary. At the time of writing, implementation support for C++20 features is spotty at best; especially with regards to `constexpr`. libFGL components and their tests make heavy use of `constexpr`. Some tests may not compile or function properly within 'incompatible' environments despite some of the components themselves being perfectly viable in a runtime state. Users of libFGL may bypass compatibility checks at their sole discretion.

**Bypassing checks:**

**All checks** can be bypassed by defining `FGL_ACKNOWLEDGE_INCOMPATIBLE_ENVIRONMENT`

**Language version checks** can be bypassed by defining `FGL_ACKNOWLEDGE_INCOMPATIBLE_STANDARD`

**Compiler version checks** can be bypassed by defining `FGL_ACKNOWLEDGE_INCOMPATIBLE_COMPILER`

---
## Debug

---
### Constexpr assert
`#include <fgl/debug/constexpr_assert.hpp>`

[assert](https://en.cppreference.com/w/cpp/error/assert)-like macro with well-defined behavior for compile-time (`constexpr` / `consteval`) evaluation.

**Quickstart**
```cpp
FGL_DEBUG_CONSTEXPR_ASSERT(1 + 1 == 2);
FGL_DEBUG_CONSTEXPR_ASSERT(1 + 1 == 3); // FATAL ERROR
// if you #define FGL_DEBUG_CONSTEXPR_ASSERT_SHORT_MACROS or FGL_SHORT_MACROS
constexpr_assert(2 + 2 == 4);
constexpr_assert(2 + 2 == 5); // FATAL ERROR
```
**Example program**
```cpp
// define enables the short "constexpr_assert"
// could also use #define FGL_SHORT_MACROS
#define FGL_DEBUG_CONSTEXPR_ASSERT_SHORT_MACROS
#include <fgl/debug/constexpr_assert.hpp>

// add two integers and assert that the result is 2
constexpr int add_and_assert_two(int a, int b)
{
	const int result{ a + b };
	constexpr_assert(result == 2);
	// without short macros, use:
	// FGL_DEBUG_CONSTEXPR_ASSERT(result == 2);
	return result;
}

int main()
{
	constexpr int a{ add_and_assert_two(1, 1) }; // OK
	constexpr int b{ add_and_assert_two(5, 5) }; // COMPILE-TIME ERROR
	int c{ add_and_assert_two(1, 1) }; // OK
	int d{ add_and_assert_two(5, 5) }; // RUNTIME ERROR (via assert)
}
```

**Interface**

**Preprocessor defininitions:**

1. Generates a fatal error if the expression (`assertion`) evaluates to boolean `false`

	`FGL_DEBUG_CONSTEXPR_ASSERT(assertion)` <sup> [*opt-in* short macro](#short-macros): `constexpr_assert(assertion)` </sup>
	- Note: function-like preprocessor macros may have difficulty parsing expressions that contain comma characters (`,`) such as `MACRO(f<a,b>());` in which case you may need to wrap the expression in parentheses: `MACRO((f<a,b>()));`

Just like the standard `assert`, `constexpr_assert` can be disabled by defining `NDEBUG` before inclusion.

If `constexpr_assert` is evaluated at runtime, the expression is forwarded to standard `assert`. If `constexpr_assert` is evaluated at compile-time and the expression is evaluated as boolean `false`, then a compile-time error will be generated via `throw` (which is illegal in a compile-time context).

**Why does this exist? Just use `assert`!**

I'm not crazy. Well, maybe a little, but that's not important right now. Calling non-`constexpr` functions at compile-time is "ill-formed, no diagnostic required". Nearly all implementations of `assert` will call an implementation-defined built-in function if the assertion fails. This means that the compiler isn't *required* to stop compilation and emit an error when `assert(false);` is evaluated at compile-time. Will nearly all compilers fail and emit an error? Yes. Do they have to? No. Hence, why this exists. Unlike `assert`, a compile-time evaluated `constexpr_assert(false);` guarentees compilation failure and should cause the compiler to emit a relevant diagnostic message.

---
### Echo
`#include <fgl/debug/echo.hpp>`

A simple way to output a string, or an expression with its result, to the [libFGL debug output stream](#outputhpp).

Echo can be a nice replacement for "print debugging" (`std::cout << "x == " << x << std::endl;`).

**Quickstart**
```cpp
FGL_DEBUG_ECHO("message"); // outputs "message"
FGL_DEBUG_ECHOV(1+1); // ouputs "1+1 == 2"
// if you #define FGL_DEBUG_ECHO_SHORT_MACROS or FGL_SHORT_MACROS
ECHO("message");
ECHOV(1+1);
```

To disable all `ECHO` output, define `NDEBUG` above the include or call `fgl::debug::output_config<fgl::debug::echo>::turn_off();`

**Example program**
```cpp
// define enables the short "ECHO" and "ECHOV"
// could also #define FGL_SHORT_MACROS
#define FGL_DEBUG_ECHO_SHORT_MACROS
#include <fgl/debug/echo.hpp>

auto add(const auto& a, const auto& b)
{
	const auto result = a + b;
	ECHOV(result);
	return result;
}

int main()
{
	ECHO("a message");
	ECHO("another message");
	ECHOV(add(1, 2));
}
```
**Example program output**
```
[ECHO] file:src/main.cpp(18:2) 'int main()
 \_____ a message
[ECHO] file:src/main.cpp(19:2) 'int main()
 \_____ another message
[ECHO] file:src/main.cpp(9:2) 'int add(int, int)
 \_____ result == 3
[ECHO] file:src/main.cpp(20:2) 'int main()
 \_____ add(1, 2) == 3
```
**Interface**

**Preprocessor defininitions:**

1. Send a `string_view` with source location to the debug output stream (`echo` priority)

	`FGL_DEBUG_ECHO(message)` <sup> [*opt-in* short macro](#short-macros): `ECHO(message)` </sup>

2. Evaluate the expression and sends the stringified expression along with its result and source location to the debug output stream (`echo` priority).

	`FGL_DEBUG_ECHOV(expr)` <sup> [*opt-in* short macro](#short-macros): `ECHOV(expr)` </sup>
	- The expression will still be evaluated if `NDEBUG` is defined.
	- Note: function-like preprocessor macros may have difficulty parsing expressions that contain comma characters (`,`) such as `MACRO(f<a,b>());` in which case you may need to wrap the expression in parentheses: `MACRO((f<a,b>()));`

**Configuration:** `fgl::debug::output_config<fgl::debug::echo>`

The `echo` specialization of `output_config` satisfies the `fgl::debug::output_channel` concept, which means it acts as the channel for `fgl::debug::echo`.

The channel default is enabled and is set to `priority::debug`, and is named `"ECHO"`.

1. Deleted constructors and destructor. Intended as a static-only (singleton) type which should never be instantiated.

	```cpp
	output_config(auto&&...) = delete;
	~output_config() = delete;
	```

2. The channel interface (satisfies `fgl::debug::output_channel`)

	- `static void turn_on()` to enable the output channel.

	- `static void turn_off()` to disable the output channel.

	- `static bool enabled()` the current enabled state.

	- `static priority get_priority()` the priority, `fgl::debug::priority::debug`.

	- `static std::string_view name()` the name of the channel (`"ECHO"`).

3. The default template for formatting `ECHOV` values

	```cpp
	template <typename T>
	[[nodiscard]] static std::string default_fmt_value(const T& value)
	```

	- Note: only `T` types with an overloaded `std::ostream` `operator<<` can be used with `ECHOV`. The implementation details of the default value formatter uses the `operator<<` of `T`, so if `T` doesn't have a compatible overload then the template synthesization will fail.

4. The template for the customizable value formatter

	```cpp
	template <typename T>
	static inline std::function<decltype(default_fmt_value<T>)> format_value
	```

5. The customizable message formatter, which defaults to `output::default_fmt_msg_src`

	```cpp
	static inline output::format_msg_src_t formatter
	```

6. The internal methods for sending `ECHO` and `ECHOV` output

	- the output method for `ECHO`

	```cpp
	static void emit(
		const std::string_view message,
		const std::source_location source = std::source_location::current())
	```

	- the output method for `ECHOV`

	```cpp
	static void emit(
		const auto result,
		const std::string_view message,
		const std::source_location source = std::source_location::current())
	```

---
### Exception Occurs
`#include <fgl/debug/exception_occurs.hpp>`

An immediate try-catch lambda wrapper which can be used to easily convert exceptions to `bool`. Useful for testing and "inline" exception handling.

It's worth noting that the idea of using exception-to-bool conversions for error handling is bad. Aside from making exception tests easier to read, this should only be employed in niche circumstances and not used as an error-handling design strategy. Exceptions should be exceptional, but some libraries are stupid and this can help contain their stupidity in a relatively clean and readable way.

**Quickstart**
```cpp
// assertions always true
assert(FGL_DEBUG_EXCEPTION_OCCURS( throw int ));
assert(FGL_DEBUG_EXCEPTION_OCCURS( func_always_throws() ));
assert(!FGL_DEBUG_EXCEPTION_OCCURS( func_never_throws() ));
assert(!FGL_DEBUG_SPECIFIC_EXCEPTION_OCCURS( std::exception, func_never_throws() ));
assert(FGL_DEBUG_SPECIFIC_EXCEPTION_OCCURS( expected_e, func_throws_expected_e() ));
assert(!FGL_DEBUG_SPECIFIC_EXCEPTION_OCCURS( different_e, func_throws_expected_e() ));
// if you #define FGL_DEBUG_EXCEPTION_OCCURS_SHORT_MACROS or FGL_SHORT_MACROS
assert(exception_occurs( throw int ));
assert(exception_occurs( func_always_throws() ));
assert(!exception_occurs( func_never_throws() ));
assert(!specific_exception_occurs( std::exception, func_never_throws() ));
assert(specific_exception_occurs( expected_e, func_throws_expected_e() ));

// note: `excpected_e` exception isn't caught and will propogate.
try{ specific_exception_occurs( different_e, func_throws_expected_e() ); }
catch(expected_e&){/* we get here */ }

// example of dealing with a stupid_lib::Thing factory
stupid_lib::ThingHolder thing_holder;

while (specific_exception_occurs(
	stupid_lib::busy,
	thing_holder = stupid_lib::make_thing()))
{
	wait_a_bit();
}

use(thing_holder);
```

**Interface**
**Preprocessor defininitions:**
1. Evaluates to boolean `true` if any exception is caught, otherwise `false`

	`FGL_DEBUG_EXCEPTION_OCCURS(expression)` <sup> [*opt-in* short macro](#short-macros): `exception_occurs(expression)` </sup>

2. Evaluates to boolean `true` if a specific exception type is caught, and `false` if no exceptions occur. Exceptions that aren't the specified type are re-thrown.

	`FGL_DEBUG_SPECIFIC_EXCEPTION_OCCURS(exception, expression)` <sup> [*opt-in* short macro](#short-macros): `specific_exception_occurs(exception, expression)` </sup>


---
### Fixme
`#include <fgl/debug/fixme.hpp>`

A simple way to leave reminders, notes, code review tags, and easily document things that are a work-in-progress. Output is sent to the [libFGL debug output stream](#outputhpp).

**Quickstart**
```cpp
FGL_DEBUG_FIX_ME; // simple output
FGL_DEBUG_FIX("a message"); // providing a message
FGL_DEBUG_FIX_THIS(expression); // message is code (will be executed)
// if you #define FGL_DEBUG_FIXME_SHORT_MACROS or FGL_SHORT_MACROS
FIX_ME;
FIX("a message");
FIX_THIS(expression);
```

To disable all `FIXME` output, define `NDEBUG` above the include or call `fgl::debug::output_config<fgl::debug::fixme>::turn_off();`

**Example program**
```cpp
#include <iostream>

// define enables the short "FIX", "FIX_ME", and "FIX_THIS"
// could also #define FGL_SHORT_MACROS
#define FGL_DEBUG_FIXME_SHORT_MACROS
#include <fgl/debug/fixme.hpp>

int add(int a, int b)
{
	FIX_THIS( return a - b; )
}

int main()
{
	FIX_ME;
	FIX_ME const int one{ 1};
	const int two{ 2 }; FIX_ME
	const int three{ add(one, two) };
	std::cout << one << " + " << two << " = " << three << std::endl;
	FIX("the author is an idiot")
}
```
**Example program output**
```
[FIXME] file:src/main.cpp(15:3) 'int main()'
[FIXME] file:src/main.cpp(16:3) 'int main()'
[FIXME] file:src/main.cpp(17:23) 'int main()'
[FIXME] file:src/main.cpp(8:3) 'int add(int, int)'
 \_____ return a - b;
1 + 2 = -1
[FIXME] file:src/main.cpp(20:3) 'int main()'
 \_____ the author is an idiot
```
**Interface**

**Preprocessor defininitions:**

1. Send a `string_view` with source location to the debug output stream (`fixme` priority)

	`FGL_DEBUG_FIX(message)` <sup> [*opt-in* short macro](#short-macros): `FIX(message)` </sup>

2. Send the source location to the debug output stream (`fixme` priority)

	`FGL_DEBUG_FIX_ME` <sup> [*opt-in* short macro](#short-macros): `FIX_ME` </sup>

3. Evaluate the expression and send the stringified expression and its source location to the debug output stream (`fixme` priority).

	`FGL_DEBUG_FIX_THIS(expr)` <sup> [*opt-in* short macro](#short-macros): `FIX_THIS(expr)` </sup>

	- The expression will still be evaluated if `NDEBUG` is defined.
	- Note: function-like preprocessor macros may have difficulty parsing expressions that contain comma characters (`,`) such as `MACRO(f<a,b>());` in which case you may need to wrap the expression in parentheses: `MACRO((f<a,b>()));`

**Configuration:** `fgl::debug::output_config<fgl::debug::fixme>`

The `fixme` specialization of `output_config` satisfies the `fgl::debug::output_handler` concept, which means it acts as both the output channel and formatter for`fgl::debug::fixme`.

The channel default is enabled and is set to `priority::debug`, and is named `"FIXME"`.

1. Deleted constructors and destructor. Intended as a static-only (singleton) type which should never be instantiated.

	```cpp
	output_config(auto&&...) = delete;
	~output_config() = delete;
	```

2. The channel interface (satisfies `fgl::debug::output_channel`)

	- `static void turn_on()` to enable the output channel.

	- `static void turn_off()` to disable the output channel.

	- `static bool enabled()` the current enabled state.

	- `static priority get_priority()` the priority, `fgl::debug::priority::debug`.

	- `static std::string_view name()` the name of the channel (`"FIXME"`).

3. The customizable formatter, which defaults to `output::default_fmt_msg_src`

	```cpp
	static inline output::format_msg_src_t formatter
	```

4. The formatter interface (satisfies `fgl::debug::output_formatter<fgl::debug::fixme>`)

	```cpp
	[[nodiscard]] static std::string format(const fixme& fixme)
	```

---
### Output
`#include <fgl/debug/output.hpp>`

For configuration and access of the libFGL debug output stream. The stream defaults to `std::cout` but can be redirected to any `ostream`.

While intended as a "debug output stream", the libFGL output system could be used for any purpose, including logging. For this reason, several output priorities and thresholds are provided (`fgl::debug::priority` enum).

Establishes a configurable type-specific output system. Defines concepts, conventions, priorities, concrete interface objects, default formatters, and a generic output configuration template.

Each type which is output can correspond to an output channel and output formatter.

**Quickstart**

Use `fgl::debug::output()` to send output to the libFGL output stream. 	Generic channels will be instantiated for every type output, and are configurable via `fgl::debug::output_config<T>`.

The generic `output_config<T>` default formatter is defined to use the `operator<<(std::ostream&, T)` but can be customized by assigning to the `std::function` member `output_config<T>::format`. Due to this default requirement, a type cannot be output using the generic formatters unless it has an appropriate `operator<<` overload.

Generic output can be disabled from appearing entirely by assigning to the `fgl::debug::generic_output_disabled` boolean.

For more control over output and type-specific formatting, the user can define static `output_channel`s and `output_formatter`s for specific types. This allows for use of `output::custom<T, T_channel, T_formatter>()`.

Concepts for static objects:

- `output_channel` must have an enable state, name, and priority.

- `output_formatter` must have a format function for `T`.

- `output_handler` is the combination of a formatter and a channel.

An `output_handler` is required to use the `output::handled<T_handler>()` method, which is equivalent to `output::custom<T, T_handler, T_handler>()`. The convention is to specialize `output_config<T>` as an `output_handler`. If that's case, you can use the `output::operator()` which is equivalent to `output::handled<T, output_config<T>>(t)`.

Whether or not a channel can output (`output::can_send<T_channel>()`) is determined by three factors: the channel's priority in relation to the `output::priority_threshold`, the channel's `enabled()` state, and the `output::enabled` boolean.

`output::priority_threshold()` gets/sets the minimum priority for channels to output. Only channels with a priority greater than or equal to this value will recieve the output stream via `output::channel_stream<T_channel>`.

Use `output::stream()` to get/set the libFGL output stream.

`output` contains several configurable formatters (`format_head`, `format_msg`, `format_msg_src`) and also defines their defaults.

Most libFGL components that interact with libFGL output will have customization settings available in a `output_config<T>` specialization.

**Example program**
```cpp
#include <string>
#include <sstream>

#include <fgl/debug/output.hpp>

// This example shows a more complex usecase of a template. For concete types,
// just specialize `output_config` to satisfy the `output_handler` concept

// the user defined type; a template in this case.
template <typename T_position = double>
struct entity
{
	std::string name;
	T_position position[3];
};

// an output channel for the user defined type
class entity_channel
{
	static inline bool m_enabled{ true };
	public:
	// should never be instantiated
	entity_channel(auto&&...) = delete;
	~entity_channel() = delete;

	// mechanisms to satisfy the output_channel concept
	static void turn_on() { m_enabled = true; }
	static void turn_off() { m_enabled = false; }
	static bool enabled() { return m_enabled; }
	static std::string_view name() { return "Entity"; }

	static fgl::debug::priority get_priority()
	{ return fgl::debug::priority::debug; }
};

static_assert(fgl::debug::output_channel<entity_channel>);

// an output formatter for the user defined type (a template in this case)
template <typename T_position>
struct entity_formatter
{
	entity_formatter(auto&&...) = delete;
	~entity_formatter() = delete;
	static std::string format(const entity<T_position>& entity)
	{
		std::ostringstream ss;
		ss
			<< entity.name << " at coordinates ("
			<< entity.position[0] << ","
			<< entity.position[1] << ","
			<< entity.position[2] << ")";
		return ss.str();
	}
};

// make sure we satisfy the formatter concept
static_assert(
	fgl::debug::output_formatter<entity_formatter<double>, entity<double>>
);

/*
now that we have a channel and formatter, we could use them directly via
`output::custom<T, T_channel, T_formatter>(t)`, but that's quite cumbersome.

For concete types, it's libFGL convention to define the channel and
formatter together in a specialization of `output_config` which satifies the
`output_handler` concept; combining the channel and formatter.

That allows us to the the `output::operator()` instead which is much easier,
and means having a unified configuration interface via `output_config<T>`.

We can abstract `entity_channel` and `entity_formatter` into a
handler object which will redirect the apporpriate calls. This way all the
`output_config<entity<T>>` synthesized objects use the same channel,
but have different (template-synthesized) formatters.

Now we can use `output::operator()` with any entity type, and all of the
configuration can be done thru the handler.
*/

template <typename T_position>
class fgl::debug::output_config<entity<T_position>>
{
	public:
	// redirect so all synethized output_config<T> types use the same channel.
	static void turn_on() { entity_channel::turn_on(); }
	static void turn_off() { entity_channel::turn_off(); }
	static bool enabled() { return entity_channel::enabled(); }
	static std::string_view name() { return entity_channel::name(); }

	static fgl::debug::priority get_priority()
	{ return entity_channel::get_priority(); }

	// call the appropriate formatter
	static std::string format(const entity<T_position>& entity)
	{ return entity_formatter<T_position>::format(entity); }
};

static_assert(
	fgl::debug::output_handler<
		fgl::debug::output_config<entity<double>>,
		entity<double>
	>
);

int main()
{
	entity player{ "Lexy", {0.0, 0.0, 0.0} };

	// set the output stream (redundant; defaults to std::cout)
	fgl::debug::output::stream(std::cout);

	// channel priority must be >= output threshold to display output
	fgl::debug::output::priority_threshold( // defaults to priority::minimum
		entity_channel::get_priority() // defined as priority::debug
	);

	// channel enabled() must return true to display output
	entity_channel::turn_on(); // redundant in this case, defaults to enabled

	// output player entity; appends a '\n' to the formatted output
	fgl::debug::output::custom<
		entity<double>,
		entity_channel,
		entity_formatter<double>
	>(player);

	// alternatively, since we satified the `output_handler` concept
	using edouble_handler_t = fgl::debug::output_config<entity<double>>;
	fgl::debug::output::handled<entity<double>, edouble_handler_t>(player);

	// but, even better since we specialized `output_config` as our handler:
	fgl::debug::output(player);

	// make sure we flush the stream since the program terminates after this
	fgl::debug::output::stream().flush();
}
```
**Example program output**
```
[Entity] Lexy at coordinates (0,0,0)
[Entity] Lexy at coordinates (0,0,0)
[Entity] Lexy at coordinates (0,0,0)
```
**Interface**

As an overview, the interface consists of: priorities, concepts, boolean switches, the `output_config` template as a generic handler, the concrete `output` structure and its instantiation as `fgl::debug::output`, its configurable members, and finally its member functions which serve as the primary interface for interacting with the output stream.

- An `enum class` used to specify the priority of channels and the threshold for output.

	```cpp
	enum class priority : unsigned char
	{
		minimum,
		debug,
		info,
		message,
		event,
		warning,
		error,
		fatal,
		maximum
	};
	```

**Concepts:**

1. Satisfied if the type provides the required information and features to be an output channel via static methods

	```cpp
	template <class T>
	concept output_channel = requires
	{
		{ T::get_priority() } -> std::same_as<priority>;
		{ T::name() } -> std::same_as<std::string_view>;
		{ T::enabled() } -> std::same_as<bool>;
		{ T::turn_on() } -> std::same_as<void>;
		{ T::turn_off() } -> std::same_as<void>;
	};
	```

2. Satisfied if the type provides a static method `format` which is used to format the output a given type as a `std::string`.

	```cpp
	template <class T_formatter, typename T_value>
	concept output_formatter = requires (const T_value& t)
	{
		{ T_formatter::format(t) } -> std::same_as<std::string>;
	};
	```

3. Satisfied if the type is a combination of a channel and formatter for a given type.

	```cpp
	template <class T, typename T_value>
	concept output_handler = output_channel<T> && output_formatter<T, T_value>;
	```


**The `output_config` template and generic output handlers:**

```cpp
template <typename T>
class output_config
```

The `output_config` class template is intended to be specialized to contain per-type output configuration settings. Ideally, in a form which satisfies `output_handler`. Most libFGL components that interact with libFGL output will have customization settings available in a specialization of `output_config`, which will allow for user-defined formatters and interaction with the channel settings. For example, `fgl::type` may specialize `fgl::output_config<fgl::type>` which will have a mutable `std::function(std::string(fgl::type)) format` member and the ability to `turn_off()` and `turn_on()` the `fgl::type` unique output channel. As this is a convention and quite flexible; interfaces may vary.

In its non-specialized form, it acts as a generic output handler (both a channel and formatter for the template argument `T`).

There's a boolean switch to disable output from all generically instantiated (non-specialized) `output_config<>` channels: `static inline bool generic_output_disabled`

Notable public members of the generic `output_config`:

1. Deleted constructors and destructor. Intended as a static-only (singleton) type which should never be instantiated

	```cpp
	output_config(auto&&...) = delete;
	~output_config() = delete;
	```

2. The interface to satisfy `output_channel`

	1. `static void turn_on()` will enable channel output

	2. `static void turn_off()` will disable channel output

	3. `static bool enabled()` will return `true` if the channel is in an enabled state and the `generic_output_disabled` boolean is `false`.

	4. `static priority get_priority()` returns `priority::info`

	5. `static std::string_view name()` returns `"GENERIC"`

3. The interface to satisfy `output_formatter<T>`

	- `static inline std::function<std::string(const T&)> format`

	- and its default `static std::string default_formatter(const T& t)` which will use the `operator<<(std::ostream&, T)`. Note that because the generic default formatter uses the stream operator overload, types without an overload cannot instantiate a generic `output_config`.

**The `output` class and scoped instantiation:**

```cpp
struct output final
```
```cpp
[[maybe_unused]] static inline output output;
```

1. boolean switch for whether or not libFGL output is enabled.

	```cpp
	static inline bool enabled{ true };
	```

2. A setter/getter for the output priority threshold. Defaults to `fgl::debug::priority::minimum`. Only output from channels whose priority is equal to or greater than this threshold should be sent to the output stream.

	```cpp
	class priority_threshold_t
	{ /* ... */ }
	static inline priority_threshold;
	```

	- setter

		```cpp
		void operator()(priority priority_threshold) noexcept
		```

	- getter

		```cpp
		[[nodiscard]] priority operator()() const noexcept
		```

	- comparison operators

		```cpp
		auto operator<=>(const priority priority) const noexcept
		```

3. A getter/setter for the output stream.

	```cpp
	class output_stream_t
	{ /* ... */ }
	static inline stream;
	```

	- setter

		```cpp
		void operator()(std::ostream& output_stream) noexcept
		```

	- getter

		```cpp
		[[nodiscard]] std::ostream& operator()() const noexcept
		```

4. Checks whether or not a channel is permitted to send output to the stream. This is determined by the `output::enabled` boolean, the channel's `enabled()` state, and the channel's priority in relation to the `output::priority_threshold()`

	```cpp
	template <output_channel T_channel>
	static bool can_send() noexcept
	```

5. A request method to be given access to the stream. Returns a `std::optional` to a refference-wrapped `std::ostream` if the channel `can_send()`. Otherwise `nullopt`.

	```cpp
	template <output_channel T_channel>
	[[nodiscard]] static
	std::optional<std::reference_wrapper<std::ostream>> channel_stream()
	```

6. Definition of default formatters

	- default formatter for the channel name output prefix

		```cpp
		[[nodiscard]] static inline std::string default_fmt_head(
			const std::string_view name)
		```

	- default formatter for a message string output

		```cpp
		[[nodiscard]] static inline std::string default_fmt_msg(
			const std::string_view message)
		```

	- default formatter for a message string and source location output

		```cpp
		[[nodiscard]] static inline std::string default_fmt_msg_src(
			const std::string_view message,
			const std::source_location source)
		```

7. Type aliases and members for each `std::function` wrapped formatter. Each type alias is defined in reference to the function signatures of the above defaults.

	- the formatter for the channel name prefix

		```cpp
		using format_head_t = std::function<decltype(default_fmt_head)>;
		static inline format_head_t format_head{ default_fmt_head };
		```

	- the formatter for a channel name and message string output

		```cpp
		using format_msg_t = std::function<decltype(default_fmt_msg)>;
		static inline format_msg_t format_msg{ default_fmt_msg };
		```

	- the formatter for a channel name, message string, and source location output

		```cpp
		using format_msg_src_t = std::function<decltype(default_fmt_msg_src)>;
		static inline format_msg_src_t format_msg_src{ default_fmt_msg_src };
		```

8. Output request on a specified channel, using a specified formatter. The output will only be sent to the stream if `can_send<T_channel>()` returns `true`.

	```cpp
	template
	<
		typename T,
		output_channel T_channel,
		output_formatter<T> T_formatter
	>
	static void custom(const T& t)
	```
	- The output will be sent to the stream with the following format:

		```cpp
		/* libFGL output stream */ << format_head(T_channel::name()) << T_formatter::format(t) << '\n';
		```

9. Output request providing the channel and formatter via an `output_handler`. The call is equivalent to `custom<T, T_handler, T_handler>(t)`.

	```cpp
	template <typename T, output_handler<T> T_handler>
	static void handled(const T& t)
	```

10. Output request providing the channel and formatter via the corresponding `output_config<T>`. The operator's `T` argument must have a corresponding `output_config<T>` which satisfies the `output_handler` concept. The call is equivalent to `handled<T, output_config<T>>(t)`.

	```cpp
	template <typename T>
	requires output_handler<output_config<T>, T>
	void operator()(const T& t) const
	```

---
### Stopwatch
`#include <fgl/debug/stopwatch.hpp>`

An accumulating stopwatch utility, used to time things and calculate statistics. Provides configurable formatters, `to_string_...` helper functions, and support for `fgl::debug::output`.

**Quickstart**
```cpp
using namespace fgl::debug;
// reserve is used by an internal vector to avoid reallocations
stopwatch sw("name", reserve);
// if no name is provided, stopwatch will be named by the source location
// if no `reserve` is provided, defaults 1000 laps

sw.start();
do_a_thing();
sw.stop();
// uses output_config<stopwatch>::format_duration
std::cout << duration_to_string(sw.previous_lap()) << std::endl;
sw.reset();

sw.start()
for (;cond();)
{
	do_a_thing();
	sw.lap(); // each lap includes cond() time
}
// stop without recording so the last lap() is accurate
sw.stop_without_record();

// send statistics to debug output stream
fgl::debug::output(sw);
```
**Example program**
```cpp
#include <iostream>
#include <thread>

#define NDEBUG // eliminate some stopwatch assertions
#include <fgl/debug/stopwatch.hpp>

int main()
{
	fgl::debug::stopwatch sw;
	std::this_thread::yield(); // warmup
	for (int i{}; i < 100; ++i)
	{
		sw.start();
		std::this_thread::yield();
		sw.stop();
	}

	std::cout << "cout: "<< sw << '\n' << std::endl;

	fgl::debug::output(sw); // outputs statistics by default

	// flush because the program terminates right after this
	fgl::debug::output::stream().flush();
}
```
**Example program output**
```
cout: yield timer: 11µs 200ns  (100 laps)

[STOPWATCH]
 \_____ Statistics: yield timer
        Number of laps: 100
        Total elapsed: 11µs 200ns
        Mean lap:      112ns
        Median lap:    100ns
        Min lap:       100ns
        Max lap:       200ns
```

**Interface** `namespace fgl::debug`


The **generic_stopwatch** utility class template

```cpp
template <typename T_clock = std::chrono::steady_clock>
requires std::chrono::is_clock_v<T_clock> && T_clock::is_steady
class generic_stopwatch
```

Constructors:

1. Explicit constructors.

	Stopwatch instances are named for human convenience. `reserve` is used by an internal vector and should equal the expected number of laps if possible, to avoid reallocations.

	1. constructs a stopwatch with a defaulted name, which is determined by a defaulted source location argument. The default name is equivalent to `std::string(sl.function_name()) + " in " + sl.file_name()`

		```cpp
		[[nodiscard]] constexpr explicit generic_stopwatch(
			const std::source_location sl = std::source_location::current(),
			const std::size_t reserve = 1000)
		```

	2. constructs a stopwatch with a provided name

		```cpp
		[[nodiscard]] constexpr explicit
		generic_stopwatch(
			const std::string_view in_name,
			const std::size_t reserve = 1000)
		```

2. Defaulted constructors

	1. the explicitly defaulted copy constructor

		```cpp
		constexpr generic_stopwatch(const generic_stopwatch& sw) = default;
		```

	2. the explicitly defaulted move constructor

		```cpp
		constexpr generic_stopwatch(generic_stopwatch&& sw) = default;
		```

Notable member type aliases:

1. the trivial clock used by the stopwatch

	```cpp
	using clock_t = T_clock;
	```

2. the `std::chrono::time_point` used by the stopwatch

	```cpp
	using time_point_t = std::chrono::time_point<clock_t>;
	```

3. the duration resolution used by the stopwatch's `clock_t`

	```cpp
	using duration_t = T_clock::duration;
	```

Notable member variables:

1. the name of the stopwatch

	```cpp
	std::string name;
	```

Notable member functions:

The stopwatch maintains internal state when `NDEBUG` is not defined. Assertions are made that things are being done in a reasonable order. For example, the stopwatch elapsed time and lap durations can't be read if the stopwatch hasn't stopped. You can't record a lap or stop the stopwatch before you start it. You can't record a lap or stop timepoint that's less than the last recorded time. Etc.

1. Start the stopwatch

	- `constexpr void start(const time_point_t time_point)`
	- `void start()` equivalent to `start(T_clock::now())`

2. Record a lap time

	- `constexpr void lap(const time_point_t time_point)`
	- `void lap()` equivalent to `lap(T_clock::now())`

3. Stop the stopwatch and record a lap time

	- `constexpr void stop(const time_point_t time_point)`
	- `void stop()` equivalent to `stop(T_clock::now())`

4. Stop the stopwatch without recording a lap time

	```cpp
	constexpr void stop_without_record() noexcept
	```

5. Reset the stopwatch, clearing all recorded laps

	```cpp
	constexpr void reset()
	```

6. Get the number of laps that have been recorded

	```cpp
	[[nodiscard]] constexpr std::size_t number_of_laps() const
	```

7. Get the duration of a given lap (within range `[0, number_of_laps)`)

	```cpp
	[[nodiscard]] constexpr
	duration_t get_lap(const std::size_t lap_number) const
	```

8. Get the duration of the previous lap (equivalent to `sw.get_lap(sw.number_of_laps()-1)`)

	```cpp
	[[nodiscard]] constexpr duration_t previous_lap() const
	```

9. Get a `const` reference to a vector containing the lap durations

	```cpp
	[[nodiscard]] constexpr
	const std::vector<duration_t>& get_all_laps() const
	```

10. Get the total elapsed time for the stopwatch

	```cpp
	[[nodiscard]] constexpr duration_t elapsed() const
	```

11. Get the total elapsed time from the start to a given lap (the sum of laps `[0, lap_number)`)

	```cpp
	[[nodiscard]] constexpr
	duration_t elapsed_to_lap(const std::size_t lap_number) const
	```

12. Get the total elapsed time from a given lap thru the last lap (the sum of laps `[ lap_number, number_of_laps() )`)

	```cpp
	[[nodiscard]] constexpr
	duration_t elapsed_since_lap(const std::size_t lap_number) const
	```
13. Get a structure containing statistics calculated from the recorded laps

	```cpp
	[[nodiscard]] constexpr statistics calculate_statistics() const
	```

	- the `statistics` structure that's returned is defined as a sub-class of the stopwatch (`generic_stopwatch<T_clock>::statistics`).

		Its constructor takes a `const&` to a vector of sorted lap durations, from which it calculates the statistics.

		```cpp
		[[nodiscard]] explicit constexpr
		statistics(const std::vector<duration_t>& sorted_laps) noexcept
		```

		Notable public members:

		```cpp
		std::size_t number_of_laps;
		duration_t total_elapsed;
		duration_t mean;
		duration_t median;
		duration_t min;
		duration_t max;
		```


`fgl::debug::stopwatch` type alias is defined for convenience as it should be the default usage for the vast majority of platforms that don't feature exotic time keeping mechanisms. (For example: network synchronization signals, high resolution timing cards, atomic clock feeds, etc. In these cases, custom types which meet the requirements of a [trivial clock](https://en.cppreference.com/w/cpp/named_req/TrivialClock) can be used).

```cpp
using stopwatch = generic_stopwatch<std::chrono::steady_clock>;
```

**Output & formatter configuration:**
```cpp
template <typename T_clock>
class output_config<generic_stopwatch<T_clock>>
```

The `generic_stopwatch<T_clock>` specialization of `output_config` satisfies the `fgl::debug::output_handler` concept, which means it acts as both the output channel and formatter for `fgl::debug::generic_stopwatch` synthesized types.

The channel default is enabled and is set to `priority::info`, and is named `"STOPWATCH"`.

`using` alias for the stopwatch type: `using stopwatch_t = generic_stopwatch<T_clock>;`

1. Deleted constructors and destructor. Intended as a static-only (singleton) type which should never be instantiated.

	```cpp
	output_config(auto&&...) = delete;
	~output_config() = delete;
	```

2. The channel interface (satisfies `fgl::debug::output_channel`)

	- `static void turn_on()` to enable the output channel.

	- `static void turn_off()` to disable the output channel.

	- `static bool enabled()` the current enabled state.

	- `static priority get_priority()` the priority, `fgl::debug::priority::info`.

	- `static std::string_view name()` the name of the channel (`"STOPWATCH"`).

3. The customizable `generic_stopwatch<T_clock>::duration_t` formatter

	- The default duration formatter

		```cpp
		[[nodiscard]] static
		std::string default_duration_formatter(stopwatch_t::duration_t duration)
		```

	- A type alias for the duration formatter member

		```cpp
		using duration_formatter_t =
			std::function<decltype(default_duration_formatter)>;
		```

	- The customizable formatter member

		```cpp
		static inline duration_formatter_t duration_formatter{
			default_duration_formatter
		};
		```

4. The customizable `generic_stopwatch<T_clock>::statistics` formatter

	- The default statistics formatter

		```cpp
		[[nodiscard]] static std::string default_statistics_formatter(
			const stopwatch_t::statistics& stats)
		```

	- A type alias for the statistics formatter member

		```cpp
		using statistics_formatter_t =
			std::function<decltype(default_statistics_formatter)>;
		```

	- The customizable formatter member

		```cpp
		static inline statistics_formatter_t statistics_formatter{
			default_statistics_formatter
		};
		```

5. The customizable `generic_stopwatch<T_clock>` formatter

	- The default stopwatch formatter

		```cpp
		[[nodiscard]] static
		std::string default_stopwatch_formatter(const stopwatch_t& sw)
		```

	- A type alias for the stopwatch formatter member

		```cpp
		using stopwatch_formatter_t =
			std::function<decltype(default_stopwatch_formatter)>;
		```

	- The customizable formatter member

		```cpp
		static inline stopwatch_formatter_t stopwatch_formatter{
			default_stopwatch_formatter
		};
		```

6. The formatter interface (satisfies `fgl::debug::output_formatter<fgl::debug::generic_stopwatch<T_clock>>`)

	```cpp
	[[nodiscard]] static std::string format(const stopwatch_t& sw)
	```

**String formatting helpers**

Loose functions that wrap calls to `output_config<generic_stopwatch<T_clock>>` formatters; some with some additional formatting.

1. `std::ostream& operator<<` overload (equivalent to `output_config`'s `stopwatch_formatter`)

	```cpp
	template <typename T_clock>
	std::ostream& operator<<(std::ostream& os, const generic_stopwatch<T_clock>& sw)
	```

2. Duration string formatter

	```cpp
	template <typename T_clock>
	[[nodiscard]] inline std::string to_string_duration(
		const typename generic_stopwatch<T_clock>::duration_t duration)
	```

3. `generic_stopwatch<T_clock>` formatters of varying verbosity.

	1. minimal (equivalent to `output_config`'s `stopwatch_formatter`)

		```cpp
		template <typename T_clock>
		[[nodiscard]] inline std::string to_string_minimal(
			const generic_stopwatch<T_clock>& sw)
		```

	2. statistics

		```cpp
		template <typename T_clock>
		[[nodiscard]] inline std::string to_string_statistics(
			const generic_stopwatch<T_clock>& sw)
		```

	3. statistics with enumerated lap durations

		```cpp
		template <typename T_clock>
		[[nodiscard]] inline std::string to_string_full(
			const generic_stopwatch<T_clock>& sw)
		```

---
## I/O

### Binary files
`#include <fgl/io/binary_files.hpp>`

Simple abstraction for read/write operations on binary files. Has `noexcept` variant wrappers and "easy-mode" helper abstractions.

**Quickstart**
```cpp
std::size_t get_file_size(path);
std::size_t get_file_size_noexcept(path);

void read_binary_file(path, output_range, bytes_to_read=0);
void read_binary_file_noexcept(path, output_range, bytes_to_read=0);
std::vector<T> read_binary_file(path);
std::vector<T> read_binary_file_noexcept(path);

void write_binary_file(path, input_range, bytes_to_write, mode=std::ios::trunc);
bool write_binary_file_noexcept(path, input_range, bytes_to_write, mode=std::ios::trunc);
void write_binary_file(path, input_range, mode=std::ios::trunc);
bool write_binary_file_noexcept(path, input_range, mode=std::ios::trunc);
```
**Example program**
```cpp
#include <iostream>
#include <cstddef> // byte
#include <array>
#include <filesystem> // path

#include <fgl/io/binary_files.hpp>

int main()
{
	constexpr std::array binary_data{ std::byte{'h'}, std::byte{'i'} };
	const std::filesystem::path file_path{ "file.bin" };

	fgl::write_binary_file(file_path, binary_data);

	const auto contents_in{ fgl::read_binary_file<char>(file_path) };

	for (const char c : contents_in)
		std::cout << c;

	std::cout << std::endl;
}
```
**Example program output**
```
hi
```
**Interface** `namespace fgl`

Note: makes use of [range constraints](#range-constraints)

1. Casting wrapper for `std::filesystem::file_size()`

	(1.1) Throws on failure
	```cpp
	template <std::integral T = std::size_t> [[nodiscard]]
	T get_file_size(const std::filesystem::path& file_path)
	```
	(1.2) Returns `std::numeric_limits<T>::max()` on failure.
	```cpp
	template <std::integral T = std::size_t> [[nodiscard]]
	T get_file_size_noexcept(const std::filesystem::path& file_path)
	noexcept
	```
2. Read binary file
	- If `bytes_to_read` is `0` it will be the size of the file obtained from `std::filesystem`. The size of the output range must be >= `bytes_to_read`.

	(2.1) Throws on failure
	```cpp
	void read_binary_file(
		const std::filesystem::path& file_path,
		fgl::contiguous_range_byte_type auto& output,
		const std::size_t bytes_to_read = 0)
	```
	(2.2) Returns `Returns `true` on success, `false` on failure.`
	```cpp
	bool read_binary_file_noexcept(
		const std::filesystem::path& file_path,
		fgl::contiguous_range_byte_type auto& output,
		const std::size_t bytes_to_read = 0)
	noexcept
	```

3. Read binary file helpers: return a `std::vector` of the file contents

	(3.1) Throws on failure.
	```cpp
	template <fgl::traits::byte_type T = std::byte> [[nodiscard]]
	std::vector<T> read_binary_file(const std::filesystem::path& file_path)
	```
	(3.2) Returns the vector on success, `std::nullopt_t` on failure.
	```cpp
	template <fgl::traits::byte_type T = std::byte> [[nodiscard]]
	std::optional<std::vector<T>>
	read_binary_file_noexcept(const std::filesystem::path& file_path)
	noexcept
	```
4. Write binary file
	- Writes `bytes_to_write` bytes from `input` range to `file_path`. Asserts that `bytes_to_write` must be less than or equal to the size of the size of the input range.

	(4.1) Throws on failure.
	```cpp
	void write_binary_file(
		const std::filesystem::path& file_path,
		const fgl::contiguous_range_byte_type auto& input,
		const std::size_t bytes_to_write,
		const std::ios::openmode mode = std::ios::trunc)
	```
	(4.2) Returns `true` on success, `false` on failure.
	```cpp
	bool write_binary_file_noexcept(
		const std::filesystem::path& file_path,
		const fgl::contiguous_range_byte_type auto& input,
		const std::size_t bytes_to_write,
		const std::ios::openmode mode = std::ios::trunc)
	noexcept
	```
5. Write binary file helpers
	- Determines `bytes_to_write` by the size of the `input` range.

	(5.1) Throws on failure.
	```cpp
	void write_binary_file(
		const std::filesystem::path& file_path,
		const fgl::contiguous_range_byte_type auto& input,
		const std::ios::openmode mode = std::ios::trunc)
	```
	(5.2) Returns `true` on success, `false` on failure.
	```cpp
	bool write_binary_file_noexcept(
		const std::filesystem::path& file_path,
		const fgl::contiguous_range_byte_type auto& input,
		const std::ios::openmode mode = std::ios::trunc)
	noexcept
	```

---
## Types

---
### C byte pointer wrapper
`#include <fgl/types/cbyte_ptr_wrapper.hpp>`

A simple casting wrapper for a non-owning byte pointer to simplify C-style byte interfaces. Implicit conversions are enabled by default, but can be disabled via boolean `false` template parameter.

**Quickstart**

implicit conversions enabled by default
```cpp
void* p{ nullptr };
fgl::cbyte_ptr_wrapper ptr(p);
void* vptr = ptr;
char* cptr = ptr;
unsigned char* ucptr = ptr;
std::byte* bptr = ptr;
```
implicit conversions disabled via template boolean
```cpp
void* p{ nullptr };
fgl::cbyte_ptr_wrapper<void*, false> ptr(p);
void* vptr = ptr.as<void*>();
char* cptr = ptr.as<char*>();
unsigned char* ucptr = ptr.as<unsigned char*>();
std::byte* bptr = ptr.as<std::byte*>();
```

**Interface** `namespace fgl`
```cpp
template
<
	fgl::traits::pointer_to_byte T,
	bool enable_implicit_conversions = true
>
struct cbyte_ptr_wrapper;
```
Constructor
```cpp
[[nodiscard]] explicit
cbyte_ptr_wrapper(T non_owning_pointer) noexcept
```
**Specializations**
- Implicit conversions disabled: `cbyte_ptr_wrapper<T, false>`
	```cpp
	template <fgl::traits::pointer_to_byte T>
	struct cbyte_ptr_wrapper<T, false>
	```
1. ```cpp
	template <fgl::traits::pointer_to_byte T_cast>
	[[nodiscard]] T_cast as() const noexcept
	```
	- reinterpret cast wrapper method

- Implicit conversions enabled: `cbyte_ptr_wrapper<T, true>`
	```cpp
	template <fgl::traits::pointer_to_byte T>
	struct cbyte_ptr_wrapper<T, true>
	: public cbyte_ptr_wrapper<T, false>
	```
1. ```cpp
	template <fgl::traits::pointer_to_byte T_cast>
	[[nodiscard]] operator T_cast() const noexcept
	```
	- implicit reinterpret cast conversion operator

---
### C types
`#include <fgl/types/ctypes.hpp>`

`using` type aliases for common C-style types.

1. ```cpp
	using cstring = const char*;
	```
	- should only be used when representing null-terminated C strings
2. ```cpp
	using cbyte = unsigned char;
	```
	- should only be used when representing raw bytes
3. ```cpp
	template <typename T, std::size_t T_length>
	using carray = T[T_length];
	```
	- Useful for making raw arrays more readable and "C plus plus - like"
	Example: `void f(carray<int, 3>& r)` versus `void f(int(&r)[3])`

---
### Range alias
`#include <fgl/types/range_alias.hpp>`

`fgl::range_alias` can be used to easily create modern and safe C++ ranges from raw pointers or other iterators. The helper factory function `make_range(iter, elements)` makes it easy to create a range from a C-style "range" (a pointer and length).

**Quickstart**
```cpp
fgl::range_alias r1(begin_iter, end_sentinel);
fgl::range_alias r2{ fgl::make_range(begin_iter, length) };
for (const auto v& : r1) // ...
```
**Example program**
```cpp
#include <cstddef> // size_t
#include <iostream> // cout
#include <ranges> // ranges::size
#include <memory> // unique_ptr

#include <fgl/types/range_alias.hpp>

int main()
{
	// heap allocate an array of 10 ints. unique_ptr so it'll be deleted.
	constexpr std::size_t length{ 10 };
	const std::unique_ptr<int[]> memory_owner(new int[length]);
	int* const ptr{ memory_owner.get() }; // raw ptr for readability

	// manual construction
	const fgl::range_alias range1(ptr, ptr+length);

	std::cout // can use in sized algorithms
		<< "size of array: "
		<< std::ranges::size(range1)
		<< std::endl;

	for (int i{ 0 }; auto& value : range1)
		value = --i; // fill memory range

	// construction via helper factory for "C-style ranges"
	const auto range2{ fgl::make_range(ptr, length) };

	for (const int i : range2)
		std::cout << i << ' '; // print memory range
}
```
**Example program output**
```
size of array: 10
-1 -2 -3 -4 -5 -6 -7 -8 -9 -10
```

**Interface** `namespace fgl`
```cpp
template
<
	std::input_or_output_iterator T_begin,
	std::sentinel_for<T_begin> T_end
>
struct range_alias
```
1. Constructor
	```cpp
	[[nodiscard]] explicit constexpr
	range_alias(const T_begin& begin, const T_end& end)
	```
2. Move constructor
	```cpp
	[[nodiscard]] explicit constexpr
	range_alias(T_begin&& begin, T_end&& end)
	```

3. Helper factory (equivalent to `range_alias(ptr, ptr + length)`)
	```cpp
	[[nodiscard]] constexpr auto make_range(
		const fgl::traits::pointer_to_non_void auto pointer,
		const std::size_t elements) noexcept
	```


Members

1. range begin iterator: `T_begin m_begin`

	(1.1)
	```cpp
	[[nodiscard]] constexpr T_begin begin() const noexcept
	```
	(1.2)
	```cpp
	[[nodiscard]] constexpr const T_begin cbegin() const noexcept
	```

2. range end sentinel: `T_end m_end`

	(2.1)
	```cpp
	[[nodiscard]] constexpr T_end end() const noexcept
	```
	(2.2)
	```cpp
	[[nodiscard]] constexpr const T_end cend() const noexcept
	```

---
### Range constraints
`#include <fgl/types/range_constraints.hpp>`

Concepts that allow for constraining ranges based on value or reference type without excessive `template` & `requires` boiler-plate noise.

**Quickstart**

FGL range constraints are conjunctions between a range concept and a type concept which constrains either the `std::ranges::range_value_t` or `std::ranges::range_reference_t`, depending on which is most applicable to the concept.

Generated concept signatures:

1. ```cpp
	template <T_range> concept
	fgl::/*range concept name*/_/*type concept name*/;
	```
2. ```cpp
	template <T_range, T_compare> concept
	fgl::/*range concept name*/_/*type concept name*/;
	```

2. ```cpp
	template <T_range, T_compare_range> concept
	fgl::/*range concept name*/_/*type concept name*/_range;
	```

Examples:

1. The concept `fgl::range_integral<T_range>` requires
	```cpp
	std::ranges::range<T_range>
	&& std::integral<std::ranges::range_value_t<T_range>>
	```

2. The concept  `fgl::contiguous_range_convertible_to<T_range, T_compare>` requires
	```cpp
	std::ranges::contiguous_range<T_range>
	&& std::convertible_to<std::ranges::range_value_t<T_range>, T_compare>
	```
3. The concept `fgl::forward_range_assignable_from_range<T_range, T_compare_range>` requires
	```cpp
	std::ranges::forward_range<T_range>
	&& std::assignable_from<std::ranges::range_reference_t<T_range>, std::ranges::range_reference_t<T_compare_range>
	```

These constraint concepts are generated from a hard-coded set of concepts: the list of supported concepts are documented in the *interface* section.

**Example program**

```cpp
#include <iostream>
#include <utility> // swap
#include <ranges> // begin, end
#include <vector>
#include <array>

#include <fgl/types/range_constraints.hpp>

void simple_range_swap(
	std::ranges::range auto& r1,
	fgl::range_swappable_with_range<decltype(r1)> auto& r2)
{
	auto r1_iter{ std::ranges::begin(r1) };
	const auto r1_end{ std::ranges::end(r1) };
	auto r2_iter{ std::ranges::begin(r2) };
	const auto r2_end{ std::ranges::end(r2) };

	while (r1_iter != r1_end && r2_iter != r2_end)
		std::iter_swap(r1_iter++, r2_iter++);
}

void print_contiguous_int_range(
	const auto& prefix,
	const fgl::contiguous_range_same_as<int> auto& range)
{
	std::cout << prefix;
	for (const int v : range)
		std::cout << v << ' ';
	std::cout << '\n';
}

int main()
{
	std::vector a{ 1,2,3,4,5,6,7 };
	std::array b{ 7,6,5,4,3,2,1 };
	print_contiguous_int_range("a: ", a);
	print_contiguous_int_range("b: ", b);
	std::cout << "swap...\n";
	simple_range_swap(a, b);
	print_contiguous_int_range("a: ", a);
	print_contiguous_int_range("b: ", b);
}
```

**Example program output**

```
a: 1 2 3 4 5 6 7
b: 7 6 5 4 3 2 1
swap...
a: 7 6 5 4 3 2 1
b: 1 2 3 4 5 6 7
```

**Interface**

**Range concepts**

- std::ranges::`range`
- std::ranges::`borrowed_range`
- std::ranges::`sized_range`
- std::ranges::`bidirectional_range`
- std::ranges::`random_access_range`
- std::ranges::`contiguous_range`
- std::ranges::`common_range`
- std::ranges::`viewable_range`

**Type concepts**

- std::`swappable_with<T&>`
- std::`assignable_from<T>`
- std::`same_as<T>`
- std::`derived_from<T>`
- std::`convertible_to<T>`
- std::`common_with<T>`
- std::`integral`
- std::`signed_integral`
- std::`unsigned_integral`
- std::`floating_point`
- fgl::traits::`not_same_as<T>`
- fgl::traits::`numeric_type`
- fgl::traits::`byte_type`
- fgl::traits::`pointer_to_byte`
- fgl::traits::`pointer_to_non_void`

**Type concepts and the `_range` suffix**

There are two categories of type constraints: "checks" that only take the range template argument (i.e. `template <T_range>` like `..._integral`) and "comparisons" which take an additional comparison type argument (i.e. `template <T_range, T_compare>` like `..._same_as`).

Each comparison concept also has a variant with the `_range` suffix, whose template arguments are `<T_range, T_compare_range>`. This variant will retrieve the constraint type from `T_compare_range`'s value or reference type; which ever is used to retrieve the type being constrained from `T_range`.

For example, `_swappable_with` constraints use the reference type

```cpp
template <typename T_range, typename T_compare>
concept fgl::/*...*/_swappable_with = /*...*/
	&& std::swappable_with
	<
		std::ranges::range_reference_t<T_range>,
		T_compare
	>;
```

therefor the `_range` suffix version will use

```cpp
template <typename T_range, typename T_compare_range>
concept fgl::/*...*/_swappable_with_range = /*...*/
	&& std::swappable_with
	<
		std::ranges::range_reference_t<T_range>,
		std::ranges::range_reference_t<T_compare_range>
	>;
```

**Why do these exist?**

Simplicity and readability.

At the time of writing, concepts cannot be passed as template arguments. libFGL's range constraints *aren't* a one-size-fits-all solution, but they can help to reduce boilerplate noise that arrises due to a common pattern: a template that constrains a range concept and the range's value or reference type.

For example, this bullshit
```cpp
template
	<std::ranges::forward_range T_range1,
	std::ranges::contiguous_range T_range2,
	std::ranges::range T_range3>
requires
	std::convertible_to<std::ranges::range_value_t<T_range1>, int>
	&& std::assignable_from
		<std::ranges::range_reference_t<T_range2>,
		std::ranges::range_reference_t<T_range1>>
	&& fgl::traits::numeric_type<std::ranges::range_value_t<T_range3>>
void f(const T_range1& r1, T_range2& r2,  const T_range3& r3);
```
can be expressed more clearly, for the sake of your fellow humans, by writing
```cpp
void f(
	const fgl::forward_range_convertible_to<int> auto& r1,
	fgl::contiguous_range_assignable_from_range<decltype(r1)> auto& r2,
	const fgl::range_numeric_type auto& r3
);
```

---
### Singleton
`#include <fgl/types/singleton.hpp>`

Type-based static singleton pattern template.

**General use of `fgl::singleton` is discouraged**. Prefer `static inline` members and methods with deleted constructors where possible.

However, this will continue to be provided because there are still cases where this singleton template design is desired.

FGL singleton can be used to create a "singleton instance" of a type that wouldn't otherwise be a singleton. It doesn't require the the template type to be a typical "singleton-exclusive" object (*Normally, types intended to be used as singletons are exclusively singletons and only have a private constructor and delete the copy and move constructors, along with the assignment operator*). Any type that satisfies [the standard `std::default_initializable` concept](https://en.cppreference.com/w/cpp/concepts/default_initializable) can be used with the singleton template. For example, `fgl::singleton<int>` is a valid singleton whose `instance()` method will return an `int&` to a static `int`.

- Note: Despite the requirement that `T` satisfy the `std::default_initializable` concept, the singleton template isn't constrained by it due to "incomplete type" issues which would complicate object definitions.

**Example program**

```cpp
#include <cassert>
#include <fgl/types/singleton.hpp>

class obj_impl
{
	FGL_PATTERN_SINGLETON_BOILERPLATE(obj);
	public:
	int data{ 0 };
};

using obj = fgl::singleton<obj_impl>;

void f1()
{
	obj::instance().data = 1337;
}

int main()
{
	f();
	assert(obj::instance().data == 1337);
}
```

**Interface** `namespace fgl`

Singleton template class type:
```cpp
template <typename T> class singleton final
```

Notable public members:

1. A static method which returns a reference to the default-initialized static `T`

	```cpp
	static T& singleton<T>::instance() noexcept;
	```

**Preprocessor defininitions:**

1. An optional "quality of life" macro to reduce common boilerplate noise for singleton-exclusive object definitions. The intent is for these definitions to be put under a `private` access specifier.

	```cpp
	#define FGL_SINGLETON_BOILERPLATE(class_name) \
		friend class fgl::singleton<class_name>;\
		class_name() = default;\
		class_name(const class_name&) = delete;\
		class_name(const class_name&&) = delete;\
		void operator=(const class_name&) = delete
	```
	- This is NOT required and should be omitted when the object needs to define a non-defaulted default constructor, or the object isn't meant to be exclusively used by the singleton template.

---
### Traits
`#include <fgl/types/traits.hpp>`

Miscellaneous type traits and concepts

**Interface** `namespace fgl::traits`

**Concepts:**

1. Satisfied when cv-removed `T` is `std::byte`, `char`, or `unsigned char`

	```cpp
	template <typename T> concept byte_type
	```

2. Satisfied when cv-removed `T` is `fgl::traits::byte_type` or `void`

	```cpp
	template <typename T> concept cbyte_type
	```

3. Satisfied when `T` satisfies either `std::integral` or `std::floating_point`

	```cpp
	template <typename T> concept nuneric_type
	```

4. Satisfied when `T1` is not the same type as `T2`

	```cpp
	template <typename T1, typename T2> concept not_same_as
	```

5. Satisfied when `T` is a pointer

	```cpp
	template <typename T> concept pointer_type
	```

6. Satisfied when `T` is a pointer to an `fgl::traits::byte_type` or `void`

	```cpp
	template <typename T> concept pointer_to_byte
	```

7. Satisfied when `T` is a pointer to any type but `void`

	```cpp
	template <typename T> concept pointer_to_non_void
	```

**Traits:**

1. `true` when  `T` is a reference and `const`-qualified

	```cpp
	template <typename T> inline constexpr bool is_const_ref
	```

2. `true` when `T` is a reference and not `const`-qualified

	```cpp
	template <typename T> inline constexpr bool is_nonconst_ref
	```

**Types:**

1. Removes pointer type and then `const` and `volatile` qualifiers from `T` (similar to [the standard `std::remove_cvref_t`](https://en.cppreference.com/w/cpp/types/remove_cvref) but for a pointer)

	```cpp
	template <typename T> using remove_cvptr_t
	```

2. Removes all pointers from `T` (similar to [the standard `std::remove_all_extents_t`](https://en.cppreference.com/w/cpp/types/remove_all_extents) but for pointers)

	```cpp
	template <typename T> using remove_all_pointers_t
	```

---
## Utility

---
### Bitwise
`#include <fgl/utility/bitwise.hpp>`

Optimized bitwise functions

**Interface** `namespace fgl::traits`

1. Returns the number of set bits (bits that are `1`)

	```cpp
	template <std::integral T_out = uint_fast8_t>
	[[nodiscard]] constexpr inline
	T_out count_set_bits(std::unsigned_integral auto bits) noexcept
	```

2. Returns the number of unset bits (bits that are `0`)

	```cpp
	template <std::integral T_out = uint_fast8_t>
	[[nodiscard]] constexpr inline
	T_out count_unset_bits(const std::unsigned_integral auto bits) noexcept
	```

---
### Circular iterator
`#include <fgl/utility/circular_iterator.hpp>`

A random access iterator which models a circular range via wrap-around behavior. By definiton, this iterator can never go out of bounds (but it can still be invalidated). A circular iterator can be indexed beyond the size of the underlying range in both positive and negative directions to traverse forwards or backwards around the circle.

For example, given a range that has `N` elements, if a circular iterator to the `0`th element is created and the iterator is incremented `N` times, its state will be identical to when it was initialized; aliasing the `0`th element. If the iterator is indexed to the `N+1`th element, it will alias the second element, equivalent to index `1`. If a circular iterator is created to the begining of a range and decremented once, it will alias the `end - 1`th element of the range. And so on.

Due to this circular nature, `iter < iter + 1` may return `false` if `iter + 1` wraps around the end of the range. The reverse holds true for subtraction.

**Quickstart**

```cpp
std::array<int, 10> r;

fgl::circular_iterator iter(r.begin(), r.end());
fgl::circular_iterator iter(r.begin(), r.end(), r.begin() + 3);
fgl::circular_iterator iter(r);
fgl::circular_iterator iter(r, r.begin() + 3);

*iter;
&*(iter -= 1) == &r.back();
&*(iter += 1) == &r.front();
iter[r.size() * 3]; // OK

auto x = iter;
auto y = iter + 2;
x - y == 8
y - x == 2
```

**Example program**

```cpp
#include <cassert>
#include <array>
#include <iostream>
#include <fgl/utility/circular_iterator.hpp>

int main()
{
	std::array<int, 5> arr{};
	auto alias{ fgl::make_circular_range_alias(arr) };
	assert(
		alias.begin()
		== fgl::circular_iterator(arr.begin(), arr.end(), arr.begin())
	);

	for (int counter{}; auto& elem : alias)
	{
		if (counter == 100)
			break;

		elem = counter++;
	}

	for (const int elem : arr)
		std::cout << elem << ", ";
	std::cout << std::endl;
}
```
**Example program output**

```
95, 96, 97, 98, 99,
```

**Interface** `namespace fgl`

```cpp
template <std::random_access_iterator T_iter>
class circular_iterator
```

Member type aliases:
```cpp
using difference_type = std::iter_difference_t<T_iter>;
using value_type = std::iter_value_t<T_iter>;
using reference_type = std::iter_reference_t<T_iter>;
using iterator_category = std::random_access_iterator_tag;
```

Constructors:

note `cursor` is the current position within the `[begin, end)` range. If no `cursor` is provided, the default cursor is `begin`.

```cpp
[[nodiscard]] constexpr explicit circular_iterator() = default;
```

```cpp
[[nodiscard]] constexpr explicit
circular_iterator(T_iter begin, T_iter end, T_iter cursor)
```

```cpp
[[nodiscard]] constexpr explicit
circular_iterator(T_iter begin, T_iter end)
```

```cpp
[[nodiscard]] constexpr explicit
circular_iterator(
	std::ranges::random_access_range auto& range,
	T_iter cursor)
```

```cpp
[[nodiscard]] constexpr explicit
circular_iterator(std::ranges::random_access_range auto& range)
```

There's also a helper function which constructs an infinite circular range alias (`std::subrange<circular_iterator<>, std::unreachable_sentinel_t>`) from a random access range.

```cpp
[[nodiscard]] constexpr
auto make_circular_range_alias(std::ranges::random_access_range auto& range)
```

Notable public methods:

- Range and cursor getters. This also means that a `circular_iterator` satisfies the `random_access_range` and `common_range` concepts.

	```cpp
	[[nodiscard]] constexpr T_iter begin() const noexcept
	```
	```cpp
	[[nodiscard]] constexpr T_iter end() const noexcept
	```
	```cpp
	[[nodiscard]] constexpr T_iter cursor() const noexcept
	```

- All the usual iterator arithmetic operators. It's worth noting that, due to its circular nature, `iter1 - iter2` and `iter2 - iter1` will both result in positive values. By definition, it's impossible for a circular iterator to go out of bounds (though, it can still be invalidated).

	1. Subtraction

		```cpp
		[[nodiscard]] constexpr
		difference_type operator-(const circular_iterator& rhs) const noexcept
		```
		```cpp
		[[nodiscard]] constexpr
		circular_iterator operator-(const difference_type n) const noexcept
		```
		```cpp
		constexpr circular_iterator& operator-=(const difference_type n) noexcept
		```
		```cpp
		constexpr circular_iterator& operator--() noexcept
		```
		```cpp
		[[nodiscard]] constexpr circular_iterator operator--(int) noexcept
		```

	2. Addition

		```cpp
		[[nodiscard]] constexpr
		circular_iterator operator+(const circular_iterator& rhs) const noexcept
		```
		```cpp
		[[nodiscard]] constexpr
		circular_iterator operator+(const difference_type n) const noexcept
		```
		```cpp
		constexpr circular_iterator& operator+=(const difference_type n) noexcept
		```
		```cpp
		constexpr circular_iterator& operator++() noexcept
		```
		```cpp
		[[nodiscard]] constexpr circular_iterator operator++(int) noexcept
		```

		- Along with the non-member operator overload:

			```cpp
			template <std::random_access_iterator T_iter>
			[[nodiscard]] constexpr circular_iterator<T_iter> operator+(
				const typename circular_iterator<T_iter>::difference_type n,
				const circular_iterator<T_iter>& iter) noexcept
			```

- Access operators

	1. Dereference operator

		```cpp
		[[nodiscard]] constexpr reference_type operator*() noexcept
		```
		```cpp
		[[nodiscard]] constexpr const reference_type operator*() const noexcept
		```

	2. Array subscript / index operator. All indexes are valid, including both positive and negative values which are greater than the size of the range.

		```cpp
		[[nodiscard]] constexpr
		reference_type operator[](const difference_type index) noexcept
		```
		```cpp
		[[nodiscard]] constexpr
		const reference_type operator[](const difference_type index) const noexcept
		```

- Comparison operators

	```cpp
	[[nodiscard]] constexpr
	bool operator==(const circular_iterator& rhs) const noexcept
	```
	```cpp
	[[nodiscard]] constexpr
	bool operator<(const circular_iterator& rhs) const noexcept
	```
	```cpp
	[[nodiscard]] constexpr
	auto operator<=>(const circular_iterator&) const noexcept = default;
	```


---
### Enumerate
`#include <fgl/utility/enumerate.hpp>`

- Currently broken due to a [libc bug](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=100639)

- May be replaced with a standard fascility in C++23

Returns an enumerated range from [fgl::zip](#zip) where the first range of the zip is an incrementing counter of integral type `T`.

**Example program**

```cpp
#include <array>
#include <vector>
#include <iostream>

#include <fgl/utility/enumerate.hpp>

int main()
{
	const std::array arr{ 1,2,3,4 };
	std::vector<float> vec(8, 3.14f);

	for (const auto& [i, v1, v2] : fgl::enumerate<unsigned int>(arr, vec))
	{
		std::cout << "arr" << '[' << i << "] == " << v1 << '\n';
		std::cout << "vec" << '[' << i << "] == " << v2 << '\n';
		std::cout << std::endl;
	}
}
```

**Example program output**

```
arr[0] == 1
vec[0] == 3.14

arr[1] == 2
vec[1] == 3.14

arr[2] == 3
vec[2] == 3.14

arr[3] == 4
vec[3] == 3.14
```

**Interface** `namespace fgl`

1. Returns a counted range iterating multiple ranges of differing types.

	```cpp
	template <std::integral T = uint32_t>
	constexpr auto enumerate(std::ranges::forward_range auto&& ... args)
	```

2. Returns a counted range iterating multiple const ranges of differing types.

	```cpp
	template <std::integral T = uint32_t>
	constexpr auto cenumerate(std::ranges::forward_range auto&& ... args)
	```

---
### Linear matrix alias

`#include <fgl/utility/linear_matrix_alias.hpp>`

Treat a linear contiguous range as if it were multi-dimensional. Support for both row major and column major.

Includes the following headers:

- [Underlying utilities](#linear-matrix-alias-utilities)

	`#include <fgl/utility/linear_matrix_alias/utilities.hpp>`

- [Static alias abstraction](#linear-matrix-alias-static-alias)

	`#include <fgl/utility/linear_matrix_alias/static.hpp>`

- [Dynamic alias abstraction](#linear-matrix-alias-dynamic-alias)

	`#include <fgl/utility/linear_matrix_alias/dynamic.hpp>`

The `static_linear_matrix_alias` provides optimal time *and* space performance when the major and size of the dimensions are constant and known at compile-time. `dynamic_linear_matrix_alias` allows the major and dimensional sizes to be set and modified at runtime. In both abstractions, the number of dimensions must be known at compile-time. The underlying `namespace fgl::linear_matrix_utilities` functions could be used for a flexible number of dimensions, but no abstraction is provided.

**Quickstart**

factory `fgl::make_matrix_alias`

Static:
```cpp
using enum fgl::major;
auto alias{ fgl::make_matrix_alias<x,y,z>(arg) }; // row by default
auto alias{ fgl::make_matrix_alias<row, x,y,z>(arg) };
auto alias{ fgl::make_matrix_alias<column, x,y,z>(arg) };
```

Dynamic:
```cpp
using enum fgl::major;
auto alias{ fgl::make_matrix_alias(arg, x, y, z) }; // row by default
auto alias{ fgl::make_matrix_alias(arg, row, x, y, z) };
auto alias{ fgl::make_matrix_alias(arg, column, x, y, z) };
```

`arg` can be either a contiguous iterator or range. If it's a range, it is bounds-checked and the size of the range must equal the product of the bounds (`x*y*z` for this example).

Using the alias:
```cpp
int counter{};
for (std::size_t i{}; i < x; ++i)
for (std::size_t j{}; j < y; ++j)
for (std::size_t l{}; l < z; ++l)
{
	alias.at({i,j,l}) = counter; // bounds-checked, may throw
	alias[{i,j,l}] = counter; // not bounds-checked
	++counter;
}
```

For both static and dynamic, the underlying iterator can be changed after construction
```cpp
// only bounds-checked if arg is a range
alias.set_iterator(arg);
```

If the alias is dynamic, the bounds and major can be changed at runtime.
```cpp
// change major
alias.switch_major(); // toggles between row and column
alias.set_major(major);

// change bounds
alias.set_bounds(bounds); // not bounds-checked

// change iterator and bounds (and optionally major)
// these are only bounds-checked if arg is a range
alias.update(arg, bounds);
alias.update (arg, bounds, major);
```

#### Linear matrix alias - Utilities

`#include <fgl/utility/linear_matrix_alias/utilities.hpp>`

**Quickstart**

```cpp
// need two ranges to contain the bounds and dimensional offsets
constexpr std::size_t dims{ 4 };
std::array<std::size_t, dims> bounds{3,3,3,3}; // tesseract (4D cube)

// calculate dimensional offsets
std::array<std::size_t, dims> offsets{};
calculate_offsets<dims, fgl::major::row>(bounds, offsets);
// or
auto offsets{ make_offsets<dims>(bounds) };

buffer[matrix_to_linear(std::array{0,2,2,0}, offsets)] = 1337;
```

"bounds" and "offsets" ranges must always be the same length.

For compile-time optimizations at the cost of synthesizing more templates, provide the number of dimensions as a template parameter `T_dimensions` which must either be `0` or match the length of the range(s).

**Example program**
```cpp
#include <cstddef> // size_t
#include <iostream> // cout, endl
#include <iomanip> // setw
#include <array>
#include <vector>

#include <fgl/utility/linear_matrix_alias/utilities.hpp>

void fill_buffer(auto& buffer)
{
	for (std::size_t i{}; auto& v : buffer)
		v = i++;
}

void print_2d_matrix(
	const auto& buffer, const auto& bounds, const auto& offsets)
{
	using fgl::linear_matrix_utilities::matrix_to_linear;
	for (std::size_t i{}; i < bounds[0]; ++i)
	{
		for (std::size_t j{}; j < bounds[1]; ++j)
		{
			std::cout
				<< std::setw(3)
				<< buffer[matrix_to_linear(std::array{i,j}, offsets)]
				<< ", ";
		}
		std::cout << '\n';
	}
	std::cout << std::endl;
}

void print_2d_matrix_both_majors(const auto& buffer, const auto& bounds)
{
	std::vector<std::size_t> offsets(bounds.size());

	using fgl::linear_matrix_utilities::calculate_offsets;

	std::cout << "row major:\n";
	calculate_offsets<0, fgl::major::row>(bounds, offsets);
	print_2d_matrix(buffer, bounds, offsets);

	std::cout << "column major:\n";
	calculate_offsets<0, fgl::major::column>(bounds, offsets);
	print_2d_matrix(buffer, bounds, offsets);
}

int main()
{
	std::vector<std::size_t> bounds{3,3}; // 2D square, 3x3
	std::vector<std::size_t> buffer(3*3);

	fill_buffer(buffer);
	print_2d_matrix_both_majors(buffer, bounds);

	// resize square 3x3 -> 4x4
	bounds[0] = 4; bounds[1] = 4;
	buffer.resize(4*4);

	fill_buffer(buffer);
	print_2d_matrix_both_majors(buffer, bounds);
}
```
**Example program output**
```
row major:
  0,   1,   2,
  3,   4,   5,
  6,   7,   8,

column major:
  0,   3,   6,
  1,   4,   7,
  2,   5,   8,
```

**Interface**

`namespace fgl`

1. an `enum class` used to specify the major

	- `fgl::major::row`

	- `fgl::major::column`

`namespace fgl::linear_matrix_utilities`

1. An assertion-helper function which is largely (or entirely) optimized away when `NDEBUG` is defined.

	Asserts that the ranges are of equal size, and then returns that size. If `T_dimensions` is `0`, asserts that the sizes of `a` and `b` are the same and returns the size. Otherwise, asserts the sizes of `a` and `b` are `T_dimensions`, and then returns `T_dimensions`.

	```cpp
	template <std::size_t T_dimensions = 0>
	[[nodiscard]] constexpr inline std::size_t assert_dimensions(
		const std::ranges::sized_range auto& a,
		const std::ranges::sized_range auto& b)
	noexcept
	```
2. Convert dimensional indexes using offsets to produce a `T_out` linear index.

	The `std::ranges::size` of `indexes` and `offsets` must be the same. `T_dimensions` must be either equal the size of each range, or `0` in which
case the number of dimensions will be dynamically determined by the sizes.

	```cpp
	template <std::size_t T_dimensions = 0, std::integral T_out = std::size_t>
	[[nodiscard]] constexpr inline T_out matrix_to_linear(
		const fgl::contiguous_range_convertible_to<T_out> auto& indexes,
		const fgl::contiguous_range_convertible_to<T_out> auto& offsets)
	noexcept
	```

3. Calculates and assigns dimensional offsets for the `T_major` of `bounds`.

	The `std::ranges::size` of `bounds` and `out_offsets` must be the same. `T_dimensions` must be either equal the size of each range, or `0` in which case the number of dimensions will be dynamically determined by the sizes.

	```cpp
	template
	<
		std::size_t T_dimensions = 0,
		major T_major = major::row,
		fgl::random_access_range_integral T_bounds,
		fgl::random_access_range_assignable_from_range<T_bounds> T_offsets
	>
	constexpr inline void calculate_offsets(
		const T_bounds& bounds,
		T_offsets& out_offsets)
	noexcept
	```

	- Helper function: returns a `std::array<T_out, T_dimensions>` containing dimensional offsets for the `T_major` of `bounds`. `T_dimensions` must equal the size of `bounds`.

		```cpp
		template
		<
			std::size_t T_dimensions,
			major T_major = major::row,
			typename T_out = std::size_t
		>
		[[nodiscard]] constexpr inline std::array<T_out, T_dimensions> make_offsets(
			const fgl::contiguous_range_integral auto& bounds)
		noexcept
		```

4. Returns `true` if any index is greater than or equal to its corresponding bound, otherwise `false`.
	```cpp
	template <std::size_t T_dimensions = 0>
	constexpr inline bool out_of_bounds(
		const fgl::contiguous_range_integral auto& indexes,
		const fgl::contiguous_range_integral auto& bounds)
	noexcept
	```

	- Helper function: throws `std::out_of_range` if any index is out of bounds

		```cpp
		template <std::size_t T_dimensions = 0>
		constexpr inline void check_index_bounds(
			const fgl::contiguous_range_integral auto& indexes,
			const fgl::contiguous_range_integral auto& bounds)
		```

#### Linear matrix alias - Static alias

`#include <fgl/utility/linear_matrix_alias/static.hpp>`

A matrix alias with compile-time constant dimensions, major, and bounds.

**Example program**
```cpp
#include <cstddef> // size_t
#include <iostream> // cout, endl
#include <array>

#include <fgl/utility/linear_matrix_alias/static.hpp>

int main()
{
	constexpr std::size_t x{3}, y{3}, z{3};
	std::array<short, x*y*z> buffer;
	for (short i{}; auto& v : buffer)
		v = i++; // fill buffer

	using enum fgl::major;
	auto alias_row{ fgl::make_matrix_alias<row, x,y,z>(buffer) };
	auto alias_column{ fgl::make_matrix_alias<column, x,y,z>(buffer) };

	/// equivalent to:
	// using iter_t = decltype(buffer.begin());
	// fgl::static_linear_matrix_alias<iter_t, row, x,y,z> alias_row(buffer);
	// fgl::static_linear_matrix_alias<iter_t, column, x,y,z> alias_column(buffer);


	std::cout << "row major: ";
	for (std::size_t i{}; i < x; ++i)
	for (std::size_t j{}; j < y; ++j)
	for (std::size_t l{}; l < z; ++l)
	{
		std::cout << alias_row[{i, j, l}] << ", ";
	}

	std::cout << "\ncolumn major: ";
	for (std::size_t i{}; i < x; ++i)
	for (std::size_t j{}; j < y; ++j)
	for (std::size_t l{}; l < z; ++l)
	{
		std::cout << alias_column[{i, j, l}] << ", ";
	}
	std::cout << std::endl;
}

```
**Example program output**
```cpp
row major: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,

column major: 0, 9, 18, 3, 12, 21, 6, 15, 24, 1, 10, 19, 4, 13, 22, 7, 16, 25, 2, 11, 20, 5, 14, 23, 8, 17, 26,
```

**Interface** `namespace fgl`

- **The static linear matrix alias abstraction.**

	A multi-dimensional alias to a contiguous range with template major, dimensions, and bounds, which enables optimizations and compile-time calculation of offsets.

	```cpp
	template
	<
		std::contiguous_iterator T_iter,
		major T_major,
		std::size_t ... T_bounds
	>
	class static_linear_matrix_alias
	```

	Factories:

	If no `T_major` is specified, it defaults to `fgl::major::row`.

	1. Created from an iterator (not bounds-checked).

		```cpp
		template <std::convertible_to<std::size_t> auto ... T_bounds>
		[[nodiscard]] constexpr auto make_matrix_alias(std::contiguous_iterator auto iter) noexcept
		```

		```cpp
		template <major T_major, std::convertible_to<std::size_t> auto ... T_bounds>
		[[nodiscard]] constexpr auto make_matrix_alias(std::contiguous_iterator auto iter) noexcept
		```

	2. Created from a contiguous range. If the size of the range does not equal the product of the dimensional bounds, a `std::invalid_argument` exception will be thrown.

		```cpp
		template <std::convertible_to<std::size_t> auto ... T_bounds>
		[[nodiscard]] constexpr auto make_matrix_alias(std::ranges::contiguous_range auto& range)
		```

		```cpp
		template <major T_major, std::convertible_to<std::size_t> auto ... T_bounds>
		[[nodiscard]] constexpr auto make_matrix_alias(std::ranges::contiguous_range auto& range)
		```


	Constructors:

	1. Constructed from a `T_iter` iterator; no bounds-checking.

		```cpp
		[[nodiscard]] explicit constexpr
		static_linear_matrix_alias(T_iter iter) noexcept
		```

	2. Constructed from a `std::ranges::contiguous_range` whose size must equal the product of the dimensional bounds, otherwise a `std::invalid_argument` exception will be thrown.

		```cpp
		[[nodiscard]] explicit constexpr
		static_linear_matrix_alias(std::ranges::contiguous_range auto& range)
		```


	Type aliases:

	1. `array_t` is a `std::array` of `size_t` whose count is determined by the number of dimensions.

	2. `difference_type` is the same as `std::iter_difference_t<T_iter>`

	Methods:

	1. Return the number of dimensions
		```cpp
		[[nodiscard]] static constexpr std::size_t dimensions() noexcept
		```

	2. Return a constant reference to the bounds
		```cpp
		[[nodiscard]] static constexpr const array_t& bounds() noexcept
		```

	3. Return a constant reference to the calculated offsets
		```cpp
		[[nodiscard]] static constexpr const array_t& offsets() noexcept
		```

	4. Return the `major` of the alias
		```cpp
		[[nodiscard]] static constexpr major get_major() noexcept
		```
		helper methods
		- `[[nodiscard]] static constexpr bool is_row_major() noexcept`
		- `[[nodiscard]] static constexpr bool is_column_major() noexcept`

	5. Returns a copy of the underyling iterator
		```cpp
		[[nodiscard]] constexpr auto iterator() const
		```

	6. Changes the underlying iterator

		- not bounds-checked
		```cpp
		constexpr void set_iterator(T_iter iter) noexcept
		```
		- bounds-checked
		```cpp
		template <std::ranges::contiguous_range T_range>
		requires std::same_as<std::ranges::iterator_t<T_range>, T_iter>
		constexpr void set_iterator(T_range& range)
		```

	7. Translates multi-dimensional indexes into the linear index for the underlying iterator

		```cpp
		[[nodiscard]] static constexpr difference_type convert_indexes(const array_t& indexes) noexcept
		```

	8. Unchecked indexing

		```cpp
		[[nodiscard]] constexpr auto& operator[](const array_t& indexes)
		```

		```cpp
		[[nodiscard]] constexpr const auto& operator[](const array_t& indexes) const
		```

	9. Bounds-checked indexing which throws if an index matches or exceeds its corresponding bound.

		```cpp
		[[nodiscard]] constexpr auto& at(const array_t& indexes)
		```

		```cpp
		[[nodiscard]] constexpr const auto& at(const array_t& indexes) const
		```

- **A static implementation helper class** which contains the information that's required for a static alias. The number of `dimensions` is determined by the `sizeof...(T_bounds)`.

	***While this is provided as a public interface for reusability reasons***, there's no need to interact with this structure directly if all you want is a `static_linear_matrix_alias`.

	```cpp
	template <major T_major, std::size_t ... T_bounds>
	struct static_linear_matrix_info
	```

	Type aliases:

	1. `using array_t = std::array<std::size_t, dimensions>;`

	Data members:

	1. `static constexpr std::size_t dimensions`
	2. `static constexpr array_t bounds`
	3. `static constexpr array_t offsets`

	Methods:

	1.	Returns `true` if any index matches or exceeds its corresponding bound, otherwise `false`
		```cpp
		[[nodiscard]] static constexpr bool out_of_bounds(const array_t& indexes)
		```

	2. throws if any index matches or exceeds its corresponding bound.
		```cpp
		static constexpr void check_bounds(const array_t& indexes)
		```

	3. translates multi-dimensional indexes to a linear index using `offsets`.
		```cpp
		template <std::integral T_out = std::ptrdiff_t>
		[[nodiscard]] static constexpr
		T_out convert_indexes(const array_t& indexes) noexcept
		```

#### Linear matrix alias - Dynamic alias

`#include <fgl/utility/linear_matrix_alias/dynamic.hpp>`

A matrix alias with runtime major and bounds, but the number of dimensions is a compile-time constant.

**Example program**
```cpp
#include <cstddef> // size_t
#include <iostream> // cout, endl
#include <vector>

#include <fgl/utility/linear_matrix_alias/dynamic.hpp>

int main()
{
	std::size_t x{ 3 }, y{ 3 }, z{ 3 };
	std::vector<short> buffer;
	buffer.resize(x*y*z);
	for (short i{}; auto& v : buffer)
		v = i++; // fill buffer

	using enum fgl::major;
	auto alias{ fgl::make_matrix_alias(buffer, row, x,y,z) };

	std::cout << "row major " << x << 'x' << y << 'x' << z << ":\n";
	for (std::size_t i{}; i < x; ++i)
	for (std::size_t j{}; j < y; ++j)
	for (std::size_t l{}; l < z; ++l)
	{
		std::cout << alias[{i, j, l}] << ", ";
	}

	++x, ++y, ++z;
	buffer.resize(x*y*z);
	for (short i{}; auto& v : buffer)
		v = i++; // fill buffer

	alias.update(buffer, {x,y,z}, column);

	std::cout << "\n\ncolumn major " << x << 'x' << y << 'x' << z << ":\n";
	for (std::size_t i{}; i < x; ++i)
	for (std::size_t j{}; j < y; ++j)
	for (std::size_t l{}; l < z; ++l)
	{
		std::cout << alias[{i, j, l}] << ", ";
	}
	std::cout << std::endl;
}
```
**Example program output**
```cpp
row major 3x3x3:
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,

column major 4x4x4:
0, 16, 32, 48, 4, 20, 36, 52, 8, 24, 40, 56, 12, 28, 44, 60, 1, 17, 33, 49, 5, 21, 37, 53, 9, 25, 41, 57, 13, 29, 45, 61, 2, 18, 34, 50, 6, 22, 38, 54, 10, 26, 42, 58, 14, 30, 46, 62, 3, 19, 35, 51, 7, 23, 39, 55, 11, 27, 43, 59, 15, 31, 47, 63,
```

**Interface** `namespace fgl`

A multi-dimensional alias to a contiguous range with runtime mutable major and bounds. The number of dimensions is a compile-time constant.

```cpp
template <std::contiguous_iterator T_iter, std::size_t T_dimensions>
class dynamic_linear_matrix_alias
```

Factories:

If no `T_major` is specified, it defaults to `fgl::major::row`.

1. Created from an iterator (not bounds-checked).

	```cpp
	[[nodiscard]] constexpr inline auto make_matrix_alias(
		std::contiguous_iterator auto iter,
		const std::convertible_to<std::size_t> auto ... bounds)
	noexcept
	```

	```cpp
	[[nodiscard]] constexpr inline auto make_matrix_alias(
		std::contiguous_iterator auto iter,
		const major major,
		const std::convertible_to<std::size_t> auto ... bounds)
	noexcept
	```

2. Created from a contiguous range. If the size of the range does not equal the product of the dimensional bounds, a `std::invalid_argument` exception will be thrown.

	```cpp
	[[nodiscard]] constexpr inline auto make_matrix_alias(
		std::ranges::contiguous_range auto& range,
		const std::convertible_to<std::size_t> auto ... bounds)
	```

	```cpp
	[[nodiscard]] constexpr inline auto make_matrix_alias(
		std::ranges::contiguous_range auto& range,
		const major major,
		const std::convertible_to<std::size_t> auto ... bounds)
	```

Constructors:

1. Constructed from a `T_iter` iterator; no bounds-checking.

	```cpp
	[[nodiscard]] explicit constexpr dynamic_linear_matrix_alias(
		T_iter iter,
		const array_t& bounds,
		const major major = major::row)
	noexcept
	```

2. Constructed from a `std::ranges::contiguous_range` whose size must equal the product of the dimensional bounds, otherwise a `std::invalid_argument` exception will be thrown.

	```cpp
	[[nodiscard]] explicit constexpr dynamic_linear_matrix_alias(
		std::ranges::contiguous_range auto& range,
		const array_t& bounds,
		const major major = major::row)
	```

Type aliases:

1. `array_t` is a `std::array` of `size_t` whose count is determined by the number of dimensions.

2. `difference_type` is the same as `std::iter_difference_t<T_iter>`

Methods:

1. Return the number of dimensions
	```cpp
	[[nodiscard]] constexpr std::size_t dimensions() const noexcept
	```

2. Return a constant reference to the bounds
	```cpp
	[[nodiscard]] constexpr const array_t& bounds() const noexcept
	```

3. Changes the bounds of the alias and and recalculates the dimensional offsets. Not bounds-checked.

	```cpp
	constexpr void set_bounds(const array_t& bounds) noexcept
	```

4. Return a constant reference to the calculated offsets
	```cpp
	[[nodiscard]] constexpr const array_t& offsets() const noexcept
	```

5. Return the major of the alias
	```cpp
	[[nodiscard]] constexpr major get_major() const noexcept
	```
	helper methods
	- `[[nodiscard]] constexpr bool is_row_major() const noexcept`
	- `[[nodiscard]] constexpr bool is_column_major() const noexcept`

6. Changes `major` of the alias and recalculates offsets.

	- explicitly set the alias major
	```cpp
	constexpr void set_major(const fgl::major new_major) noexcept
	```

	- toggle between majors (note: this still performs an offset recalculation every time)
	```cpp
	constexpr void switch_major() noexcept
	```

7. Returns a copy of the underyling iterator
	```cpp
	[[nodiscard]] constexpr auto iterator() const noexcept
	```

8. Changes the underlying iterator.

	- not bounds-checked
	```cpp
	constexpr void set_iterator(T_iter iter) noexcept
	```

	- bounds-checked. The size of the range must equal the product of the dimensional bounds, otherwise a `std::invalid_argument` exception will be thrown.
	```cpp
	template <std::ranges::contiguous_range T_range>
	requires std::same_as<std::ranges::iterator_t<T_range>, T_iter>
	constexpr void set_iterator(T_range& range)
	```

9. Simultaneously changes the iterator, bounds, and (optionally) major.

	- Not bounds-checked
	```cpp
	constexpr void update(T_iter iter, const array_t& bounds) noexcept
	```

	```cpp
	constexpr void update(
		T_iter iter,
		const array_t& bounds,
		const fgl::major new_major) noexcept
	```

	- Bounds-checked. The size of the range must equal the product of the dimensional bounds, otherwise a `std::invalid_argument` exception will be thrown.
	```cpp
	template <std::ranges::contiguous_range T_range>
	requires std::same_as<std::ranges::iterator_t<T_range>, T_iter>
	constexpr void update(T_range& range, const array_t& bounds)
	```

	```cpp
	template <std::ranges::contiguous_range T_range>
	requires std::same_as<std::ranges::iterator_t<T_range>, T_iter>
	constexpr void update(
		T_range& range,
		const array_t& bounds,
		const fgl::major new_major)
	```

10. Translates multi-dimensional indexes into the linear index for the underlying iterator

	```cpp
	[[nodiscard]] constexpr
	difference_type convert_indexes(const array_t& indexes) const noexcept
	```

11. Unchecked indexing

	```cpp
	[[nodiscard]] constexpr auto& operator[](const array_t& indexes)
	```

	```cpp
	[[nodiscard]] constexpr const auto& operator[](const array_t& indexes) const
	```

12. Bounds-checked indexing which throws if an index matches or exceeds its corresponding bound.

	```cpp
	[[nodiscard]] constexpr auto& at(const array_t& indexes)
	```

	```cpp
	[[nodiscard]] constexpr const auto& at(const array_t& indexes) const

---
### Make byte array
`#include <fgl/utility/make_byte_array.hpp>`

A utility to construct and fill a `std::array` of `std::byte` from a cstring literal (excluding null-terminator)

**Example program**

```cpp
#include <iostream>
#include <cstddef> // byte, to_integer

#include <fgl/utility/make_byte_array.hpp>

int main()
{
	constexpr auto barray1 = fgl::make_byte_array("\x01\xA0\xAF\xFF hello");
	for (const std::byte b : barray1)
		std::cout << "0x" << std::to_integer<int>(b) << ' ';

	std::cout << std::endl;

	using namespace fgl::literals;
	constexpr auto barray2 = "\x01\xA0\xAF\xFF goodbye"_bytes;
	for (const std::byte b : barray2)
		std::cout << "0x" << std::to_integer<int>(b) << ' ';
}
```
**Example program output**
```
0x1 0x160 0x175 0x255 0x32 0x104 0x101 0x108 0x108 0x111
0x1 0x160 0x175 0x255 0x32 0x103 0x111 0x111 0x100 0x98 0x121 0x101
```

**Interface** `namespace fgl`

1. Creates, fills, and returns a `std::array` of `std::byte>` from a cstring literal (excluding the null terminator)

	```cpp
	template <std::size_t T_length> [[nodiscard]] constexpr
	std::array<std::byte, T_length-1> make_byte_array(
		fgl::carray<const char, T_length>& cstr)
	```

**Byte array literal** `namespace fgl::literals`

1. A `consteval` cstring literal operator wrapper for `fgl::make_byte_array`

	```cpp
	consteval auto operator"" _bytes()
	```

---
### Matrix
`#include <fgl/utility/matrix.hpp>`

A multi-dimensional row-major array; not a mathematical matrix. Combines the behavior of a [static linear matrix alias](#linear-matrix-alias-static-alias) with a `std::array`.

**Quickstart**
Inherets `std::array` and it's interface.
```cpp
fgl::matrix<int, x,y,z,w> matrix;
matrix[{0,0,0,0}] = 0;
matrix.at({0,0,0,0}) = 0;

matrix.linear_index(0) = 0;
matrix.linear_at(0) = 0;

std::array<int, x*y*z*w>& array{ matrix.linear_array() };
```
**Example program**
```cpp
#include <cstddef> // size_t
#include <iostream> // cout, endl
#include <iomanip> // setw

#include <fgl/utility/matrix.hpp>

int main()
{
	constexpr std::size_t x{ 5 }, y{ 5 };
	fgl::matrix<short, x, y> matrix;

	for (short i{}; auto& v : matrix) // using std::array's begin/end
		v = i++; // fill matrix (row major)

	std::cout << "Matrix " << x << 'x' << y << ":\n";
	for (std::size_t i{}; i < x; ++i)
	{
		for (std::size_t j{}; j < y; ++j)
		{
			std::cout << std::setw(3) << matrix[{i, j}] << ", ";
		}
		std::cout << std::endl;
	}
}
```
**Example program output**
```
Matrix 5x5:
  0,   1,   2,   3,   4,
  5,   6,   7,   8,   9,
 10,  11,  12,  13,  14,
 15,  16,  17,  18,  19,
 20,  21,  22,  23,  24,
```

**Interface** `namespace fgl`]

Linear `std::array` aggrigate initialization only. No constructors.

Type aliases:

1. `dimensional_t` is a `std::array` of `std::size_t` whose size is equal to the number of dimensions.

2. `base_t` is the underlying (inhereted) linear `std::array`

Methods:

1. returns a reference to the underlying linear array. Equvalent to `static_cast<base_t&>(matrix)`.
	```cpp
	[[nodiscard]] constexpr const base_t& linear_array() const noexcept
	```

	```cpp
	[[nodiscard]] constexpr const base_t& linear_array() const noexcept
	```

2. Unchecked index into the linear `std::array` via its `operator[]`

	```cpp
	[[nodiscard]] constexpr auto& linear_index(const std::size_t index)
	```

	```cpp
	[[nodiscard]] constexpr const auto& linear_index(const std::size_t index) const
	```

3. Bounds-checked index into the linear `std::array` via its `at()` method

	```cpp
	[[nodiscard]] constexpr auto& linear_at(const std::size_t index)
	```

	```cpp
	[[nodiscard]] constexpr const auto& linear_at(const std::size_t index) const
	```

3. Unchecked index into the multi-dimensional matrix

	```cpp
	[[nodiscard]] constexpr auto& operator[](const dimensional_t& indexes)
	```

	```cpp
	[[nodiscard]] constexpr const auto& operator[](const dimensional_t& indexes) const
	```

4. Bounds-checked index into the multi-dimensional matrix. Will throw `std::out_of_range` if any index matches or exceeds its corresponding dimensional bound.

	```cpp
	[[nodiscard]] constexpr auto& at(const dimensional_t& indexes)
	```

	```cpp
	[[nodiscard]] constexpr const auto& at(const dimensional_t& indexes) const
	```

---
### Random
`#include <fgl/utility/random.hpp>`

A simplifying object template abstraction for the C++ STL pseudo-random-number generation fascilities which prioritizes ease-of-use.

**Example program**

```cpp
#include <iostream> // cout, endl
#include <fgl/utility/random.hpp>

int main()
{
	using std::cout, std::endl, fgl::random;

	// defualts: uint_fast64_t, between type min and max, uniform dist.
	random rng_a;
	cout << "between min and max uint_fast64_t: " << rng_a() << endl;

	random<int> rng_b(-9, 9);
	cout << "between -9 and 9 int: " << rng_b() << endl;

	random<uint_least8_t> rng_c;
	cout
		<< "between min and max uint_least8_t: "
		<< static_cast<unsigned int>(rng_c())
		<< endl;
	// typecast because underlying 8bit type is often a char

	random<float> rng_d(3.14f, 69.69f);
	cout << "between 3.14 and 69.69 float: " << rng_d() << endl;

	// changing distribution
	random<double, std::normal_distribution<double>> rng_e;
	cout << "between min and max double, normal dist: " << rng_e() << endl;
}
```

**Example program output**

```
between min and max uint_fast64_t: 6909273070770195903
between between -9 and 9 int: -3
between min and max uint_least8_t: 156
between 3.14 and 69.69 float: 28.5469
between min and max double, normal dist: -inf
```

**Interface** `namespace fgl`

```cpp
template
<
	fgl::traits::numeric_type T = uint_fast64_t,
	class T_distribution =
		std::conditional
		<
			std::integral<T>,
			std::uniform_int_distribution<T>,
			std::uniform_real_distribution<T>
		>::type,
	class T_generator = std::mt19937_64,
	const std::size_t T_table_size = 8
>
struct random
```

1. Constructor

	```cpp
	[[nodiscard]] explicit random(
		const T min = std::numeric_limits<T>::min(),
		const T max = std::numeric_limits<T>::max(),
		const std::random_device::result_type seed = (std::random_device{}())
	```
Members:

1. Call operator which returns a random `T` between `min` and `max` from a `T_distribution` invoked with an order-shuffled `T_generator`

	```cpp
	[[nodiscard]] T operator()()
	```

---
### Sleep
`#include <fgl/utility/sleep.hpp>`

High resolution, short time scale sleep functions.

**Prefer the use of `std::this_thread::sleep_for` and `std::this_thread::yield` where more appropriate**.

However, there are use cases where this header's functions may be applicable. `nano_sleep` is implemented as a nanosecond resolution hot loop which may be used for sub-`yield` timing (`+~400ns` under ideal conditions). `micro_sleep` is implemented as a microsecond resolution `yield`ing loop which may be used for sub-`sleep_for` timing (`+~3μs` under ideal conditions). The only guarentee is that the functions will return some time after requested duration. The exact delays are subject to the whims of the scheduler, platform, and `std::chrono::steady_clock` implementation.

For accuracy over longer time scales, the standard provided sleep functions can be layered on top of these functions. For example: `sleep_until` or `sleep_for` can be used to bring the required resolution within the appropriate timeframe for `micro_sleep`, which can be used to further reduce the timeframe until `nano_sleep` should take over. With some testing to figure out the ranges for the ideal timeframes, this technique can feisably provide very good sleep accuracy on certain platforms and environments.

**Quickstart**

```cpp
fgl::nano_sleep(std::chrono::nanoseconds(500)); // hot loop
fgl::micro_sleep(std::chrono::microseconds(2)); // yielding loop
```

**Interface** `namespace fgl`

1. A nano-second scale sleep, implemented as a non-`yield`ing hot spin loop

	```cpp
	inline void nano_sleep(const std::chrono::nanoseconds duration) noexcept
	```

2. A micro-second scale sleep, implemented as a `yield`ing loop

	```cpp
	inline void micro_sleep(const std::chrono::microseconds duration) noexcept
	```

---
### Zip
`#include <fgl/utility/zip.hpp>`

Note: depending on performance characteristics, this may be removed in favor of C++23 `std::views::zip`

An interface to easily create a range which iterates multiple ranges of different types at once. The range is composed of an FGL internal zip iterator and a length-based end sentinel. Dereferencing the iterator returns a tuple of references or values from the zipped ranges' iterators, in the order the ranges were passed to `fgl::zip`. In the case of `fgl::czip`, they are `const`-qualified references.

**Quickstart**

```cpp
	for (const auto& [a, b] : zip(a_vec, b_arr))
		a += b; // mutable references like *begin()

	int sum{ 0 };
	for (const auto& [a, b] : czip(a_vec, b_arr))
		sum += a + b; // const references like *cbegin()

	// explicit length so it doesn't need to check each range
	for (const auto& [a, b, c]
		: czip(shortest_range.size(), shortest_range, longer, longer))
```

**Example program**

```cpp
#include <iostream>
#include <array>
#include <vector>
#include <map>

#include <fgl/utility/zip.hpp>

consteval int calc()
{
	int sum{ 0 };
	std::array a1{ 0,1,2,3,4 };
	std::array a2{ 5,6,7,8,9,10,2000,4500001 };
	std::array a3{ 11,12,13,14,15,16,251000,4500,145101 };
	for (const auto& [a1_v, a2_v, a3_v] : fgl::czip(a1, a2,a3))
		sum += a1_v + a2_v + a3_v;
	return sum;
}

int main()
{
	constexpr auto result{ calc() };
	std::cout << result << std::endl;

	std::vector<float> vec(20, 3.14f);
	std::map<int, float> map{ {1, 1.1f}, {2, 2.2f}, {3, 3.3f} };
	for (const auto& [v1, m, v2] : fgl::zip(vec, map, vec))
	{
		const auto& [m_k, m_v]{ m }; // cant nest structured bindings
		std::cout<< v1 << " += " << m_k << " + " << m_v << " = ";
		v1 += m_k + m_v;
		std::cout << v2 << '\n'; // same element as v1
	}
}
```

**Example program output**

```
110
3.14 += 1 + 1.1 == 5.24
3.14 += 2 + 2.2 == 7.34
3.14 += 3 + 3.3 == 9.44
```

**Interface** `namespace fgl`

Type aliases

1. The sentinel type for the zipped range and internal zip iterator.

	`zip_sentinel_t`

Function templates

`fgl::zip` and `fgl::czip` function templates return a length-based range which iterates over multiple ranges of different types.

1. Dereferencing the zip iterator will return a `std::tuple` of mutable references (iterators obtained via `std::begin()`)

	(1.1)
	```cpp
	template <std::ranges::forward_range ... T_ranges>
	constexpr auto zip(const std::integral auto length, T_ranges&& ... args)
	```
	(1.2)
	```cpp
	template <std::ranges::forward_range ... T_ranges>
	constexpr auto zip(T_ranges&& ... args)
	```

2. Dereferencing the zip iterator will return a `std::tuple` of `const` references (iterators obtained via `std::cbegin()`)

	(2.1)
	```cpp
	template <std::ranges::forward_range ... T_ranges>
	constexpr auto czip(const std::integral auto length, T_ranges&& ... args)
	```

	(2.2)
	```cpp
	template <std::ranges::forward_range ... T_ranges>
	constexpr auto czip(T_ranges&& ... args)
	```

- (1.1, 2.1) the zip's end sentinel is determined by the `length` parameter which is static cast to `zip_sentinel_t`. Assertion: length must be less than or equal to the length of the shortest range.

- (1.2, 2.2) The zip's end sentinel is determined by the shortest range.
