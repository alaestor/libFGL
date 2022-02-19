The Future Gadget Laboratory Library
---

**libFGL** — For use with Tempus Edax Rerum projects.

A modern C++ header-only library (that will someday become a module).

Breaking changes are a feature. No support. No guarentees. Not for production use.

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
  - [Read me first](#read-me-first)
  - [Debug](#debug)
    - [Constexpr assert](#constexpr-assert)
    - [Echo](#echo)
    - [Fixme](#fixme)
    - [Output](#output)
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
    - [Enumerate](#enumerate)
    - [Make byte array](#make-byte-array)
    - [Random](#random)
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

## Read me first

The documentation found here is a high-level overview of libFGL's features which provides explanations and examples. The most accurate and up-to-date **reference material** will always be the code and comments in the headers themselves. libFGL's headers are written to be read and have meaningful comments and symbol names: the interfaces and abstractions are usually concise and relatively self-explanatory. The source code for small example programs are also included in the header comments.


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

**Configuration singleton:** `fgl::debug::echo::config.instance()`

Notable public members:

1. Set a custom formatter for fixme.
	```cpp
	void change_formatter(const echo_formatter_t& custom_formatter)
	```
	- `ECHOV` will always append `== (the result of the expression)` to the message.

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

**Configuration singleton:** `fgl::debug::fixme::config.instance()`

Notable public members:

1. Set a custom formatter for fixme.
	```cpp
	void change_formatter(const fixme_formatter_t& custom_formatter)
	```

### Output
`#include <fgl/debug/output.hpp>`

For configuration and access of the libFGL debug output stream. The stream defaults to `std::cout` but can be redirected to any `ostream`.

Defines a default formatter which is used by other libFGL components (such as [Echo](#echo) and [Fixme](#fixme)).

Defines priority levels which can be used to exclude output from sources that are bellow a configurable threshold. User-defined priorities are allowed.

**Example program**
```cpp
#include <fgl/debug/output.hpp>

int main()
{
	// make a reference to the output configuration singleton
	auto& config{ fgl::debug::output::config::instance() };

	// redirect libFGL debug output to std::cerr
	config.change_output_stream(std::cerr);

	// only output warnings and above
	using priority = fgl::debug::output::priority_e;
	config.change_output_threshold(priority::warn);

	// send something to the output stream
	config.output_stream() << "Hello, world!" << std::endl;
}
```
**Example program output**
```
Hello, world!
```
**Interface**

**Configuration singleton:** `fgl::debug::output::config.instance()`

Notable public members:

1. Get the priority threshold
	```cpp
	priority_e priority_threshold() const noexcept
	```

2. Change the priority threshold (only output from `priority` sources and above should be sent to the debug output stream)
	```cpp
	void change_priority_threshold(const priority_e priority) noexcept
	```

3. Check if `priority` is greater than or equal to the current threshold
	```cpp
	bool above_priority_threshold(const priority_e priority) const noexcept
	```

4. Change the debug output stream

	(4.1)
	```cpp
	void change_output_stream(std::ostream* const output_stream) noexcept
	```
	(4.2)
	```cpp
	void change_output_stream(std::ostream& output_stream) noexcept
	```

5. Get a reference to the debug output stream

	```cpp
	std::ostream& output_stream() const
	```

6. The default formatter used by other libFGL components
	```cpp
	static std::string default_formatter(
		const priority_e priority,
		const std::string_view message,
		const std::source_location source)
	```

**Priorities and formatters** `namespace fgl::debug::output`
Output proprity is defined as `enum priority_e : uint_fast8_t`. As a convention, libFGL components that use the debug output will check to ensure that the message priority is `above_priority_threshold` (3). If `true`, the message should be sent to a formatter, and its return value sent to the output stream. Only the component and formatters are aware of and handle message priorities.

The lowest priority is `priority_e::ALL`, and the highest is `priority_e::MAX`. User-defined output should only use `ALL`, `debug`, `info`, `warn`, `error`, and `MAX` priorities, or a custom user-defined priority. All other priorities are intended for internal use. Custom priorities should be defined relative to and greater than `priority_e::MAX`, and can be specially handled by defining custom formatters which recieve the message priority as an argument.

Use the `default_formatter` member of the internal configuration singleton as an example when defining custom formatters.

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

FGL singletons don't require the the template type to be a typical "singleton-exclusive" object (*Normally, types intended to be used as singletons are exclusively singletons and only have a private constructor and delete the copy and move constructors, along with the assignment operator*). Any type that satisfies [the standard `std::default_initializable` concept](https://en.cppreference.com/w/cpp/concepts/default_initializable) can be used with the singleton template. For example, `fgl::singleton<int>` is a valid singleton whose `instance()` method will return an `int&` to a static `int`.

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

5. Satisfied when `T` is a pointer to an `fgl::traits::byte_type` or `void`

	```cpp
	template <typename T> concept pointer_to_byte
	```

6. Satisfied when `T` is a pointer to any type but `void`

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
