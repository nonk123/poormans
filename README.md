# poormans

The kind of shit a morgue IT technician comes up with when there's nothing to do, but their computer is able to compile C and doesn't support OpenGL. Text-mode graphics inside the Windows console for all kinds of Pentium-III machines, contained within a single header file.

Refer to [toy program sources](src/test.c) for quickstart.

## Installation

Use `FetchContent` with CMake to include this library in your project:

```cmake
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE) # shows `git clone` progress for the impatient

FetchContent_Declare(poormans
    GIT_REPOSITORY https://github.com/nonk123/poormans.git
    GIT_TAG master) # or any other tag
FetchContent_MakeAvailable(poormans)

# Create your executable target, e.g.:
add_executable(myProgram ...)
# Link against `poormans`:
target_link_libraries(myProgram PRIVATE poormans)
```

## Usage

In your C code, use the following boilerplate/template for `main.c` or such:

```c
// Don't forget to compile the function definitions from `poormans`:
#define POOR_IMPLEMENTATION
#include "poormans.h"

int main(int argc, char* argv[]) {
    for (poor_init(); poor_running(); poor_tick()) {
        // Actual ticking/drawing logic goes here.
    }
    return EXIT_SUCCESS;
}
```

You'll most likely want to use `poor_at(x, y)` to put character cells on the screen. Set `chr` field to display an ASCII character inside that cell. Modify `fg` & `bg` to one of the `POOR_*` color constants to add colors to your cells. The screen is cleared with whitespace automatically at the start of each frame.

You can use `poor_key_down(scancode)` and `poor_key_pressed(scancode)` to respond to keypresses. Pass one of the `POOR_*` scancode constants to `poor_key_down` to check if key is held down at the moment, or to `poor_key_pressed` to check if the key was just pressed. `poor_key_down` will signal continuously as long as the key is held, while `poor_key_pressed` will trigger once and then continuously according to system's key repeat interval.

TODO: implement & document mouse input.
