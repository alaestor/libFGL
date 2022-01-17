# Future Gadget Laboratory Library

**libFGL** — For use with Tempus Edax Rerum projects.

A modern C++ header-only library (that will someday become a module).

Breaking changes are a feature.

No support. No guarentees. Not for production use.


# Extensions

- WIP [libFGL-vulkan]() - a simple and modern interface for using Vulkan compute shaders.

WIP repo: [KJNeko's LearningVulkan](https://github.com/KJNeko/LearningVulkan)

# Quick-start guide

Download libFGL to where ever you'd like.

For [clang](https://clang.llvm.org/docs/ClangCommandLineReference.html#include-path-management) and [gcc](https://gcc.gnu.org/onlinedocs/gcc/Directory-Options.html), pass `-I"path/to/libFGL/include"` when compiling.

For [MSVC](https://docs.microsoft.com/en-us/cpp/preprocessor/hash-include-directive-c-cpp?view=msvc-170), pass `/I"path/to/libFGL/include"` when compiling.

In your source file, include the components you want using `<>` syntax:
```cpp
#include <fgl/utility/zip.hpp>

// silly example code using fgl::zip
int sum_two_int_vecs(const std::vector<int>& a, const std::vector<int>& b)
{
	int sum{ 0 };
	for (const auto& [av, bv] : fgl::zip(a, b))
		sum += a + b;
	return sum;
}
```

libFGL is header-only and doesn't require compilation. Just include the components you want and you're done. However, you should probably compile and run the unit tests to make sure everything's behaving properly in your environment.

libFGL extensions can require elaborate dependencies and compilation steps. Refer to the extension's documentation.

# Project structure & naming

### Extensions

libFGL extensions are built ontop of libFGL but are components that usually require compilation and third-party dependencies (like SDKs or non-FGL libraries). Extensions share the libFGL include structure and conventions.

For example: `g++ -I"path/to/libFGL" -I"path/to/libFGL-extension"` ...

```cpp
#include <fgl/component.hpp> // from the header-only "core" libFGL
#include <fgl/extension.hpp> // from a separately compiled libFGL extension
```

### Just give me everything

The `include/fgl.hpp` includes all top-level `include/fgl/*.hpp` headers which should include *almost* the entire library; everything except experimental and opt-in features. `include/FGLM` includes `include/fgl.hpp` but also all opt-in features like `FGL_SHORT_MACROS` and exposes `using namespace fgl::literals;`. libFGL extensions must be included separately.

### `include` structure

Each include folder has a corresponding `.hpp` file of the same name which includes all header files in that folder. For example `FOLDER.hpp` will include all `FOLDER/*.hpp` files.


### Special names
Any file, folder, or symbol with these names is internal to the FGL library. You should never explicitly include these files or use these components directly.
- `experimental` / `WIP` / `unfinished`
- `internal` / `details`
- `test`
- any name prefixed with an underscore

# Testing
### How to run tests

Remove the `.DEFAULT` suffix from `tup.config.DEFAULT`. This file contains system-specific paths and user preferences.

Use [the `tup` build-system](https://gittup.org/gittup/) by executing `tup` in the directory that contains `Tupfile.ini`. The top-level `Tupfile` is configured to build and run all tests. If `tup` completes without warnings or errors, then the library should be fully-functional on the platform in question.

# Everything bellow this heading will be moved to the wiki whenever we get around to it.

## Writing tests

### Tests guidelines.

Each component of the library, where applicable, will have a unit test in the `test/` directory with a folder name that matches the include structure with separators replaced with underscores. For example, `include/fgl/debug/fixme.hpp` corresponds with `test/fgl_debug_fixme`.

Tests should have minimal includes and namespace polution. Test programs return `EXIT_SUCCESS` to signal that the test(s) have passed. Tests should strive to use compile-time `static_assert` and `consteval` functions where ever possible. Where not possible, use `assert`. Where not possible, a test failure should cause the program to return code `EXIT_FAILURE`. Tests should not use standard input/out (i.e. `cout`, `cin`). Each test (i.e. assertion) should be granular and represent a minimal subset of component behavior in order to improve debugability. Don't accumulate failures where possible; prefer to fail hard and fast. Use descriptive variable and function names. The intent and conditions of a test should be self-evident in code or at the very least clearly documented by comments.

### Test tup files & dependencies

Each test (`test/fgl_component/`) will contain a `Tupfile` which defines rules for compilation, linking, and running of the test. Optionally, a test can have a `Tuprules.tup` file which defines test prerequisites / test dependencies based on folder name.

### Walkthrough of a test's build files
###### Tupfile
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

###### Tuprules.tup
Using `test/fgl_utility_enumerate` as an example:
```sh
TEST_PREREQUISITE= $(TEST_DIR)/<fgl_utility_zip> $(TEST_DIR)/<fgl_types_traits>
```
Here, `fgl_utility_enumerate` requires that the `fgl_utility_zip` and `fgl_types_traits` tests pass before `fgl_utility_enumerate`'s test is run.

# Style guidelines
Just a bunch of examples. I'm a text file not a cop.

Favor at-a-glance readability and sane indentation over strict rule adherence.

```cpp
class whispering_snake_case
{};

bool init{ value };

bool multline_init{
	value1
	&& value2
};

const auto thing {
	[]() -> thing_t
	{
		thing_t thing{ default value };
		mutate(thing);
		return thing;
	}()
};
```

```cpp
void function_definition(arg) const noexcept(simple conidition)
{ one line body; }

[[nodiscard]] constexpr inline
void function_definition(
	multiline,
	arg)
const noexcept(
	complicated
	&& noexcept condition)
{
	multiline_body; // also if the signature is multiline
}

function_call(
	multiline
		really_long,
	arg
);
```

```cpp
constructor(arg1, arg2)
: m_arg1(arg1), m_arg2(arg2), //... to column limit (~80)
{} // empty body

constuctor(
	arg1,
	arg2,
	arg3)
:
	m_arg1(arg1),
	m_arg2(arg2),
	m_arg3(arg3) //... over column limit (~80)
{
	// body code
}
```

```cpp
// use T_ prefix
template <typename T>
template <typename ... Ts> // OK, but prefer T_descriptions
template <typename T_thing>
template <typename ... T_things>
```

```cpp
// a worst-case example
template
<
	typename ... T_things
>
[[nodiscard]] constexpr inline
const super duper really really really really long long int&
function_with_lots_of_boilerplate(
	super duper really really really really long long int&
		multiline_args1,
	multiline_args2,
	T_things& ... things)
const noexcept(
		bunch of
		&& shit)
	:
		member(),
		initializers()
{
	body;
}
// still decently readable? but please, do better...
```
