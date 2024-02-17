# windows-novel-hook
Novel way to hook Windows API functions

# What is it?
A few months ago, I wanted to create a monitoring program which would log Windows API function calls by hooking every Windows API function imported by a program. After some research, I concluded that this was difficult as Detours, my hooking library of choice, requires the function signatures of the target functions to be known before the function can be hooked, meaning I could not just resolve the names of the imported functions and hook them from name or offset alone. Ultimately, if my goal was to log all Windows API function calls, I would have to either find and write a function signature for every single Windows API function, or modify the Windows API DLLs to log when each function was called. This seemed like an impossible task.

However, I came up with a solution by cross-referencing the names of the imported functions with the Windows documentation, which often includes the relevent function signatures. I extracted these signatures from the Windows documentation, compiled them into a DLL and loaded the new DLL into the process, finally allowing me to hook the functions dynamically at runtime.

This solution works however it is certainly novel and not completely reliable at the moment as the documentation parsing, namely the heuristic signature extraction technique could be improved, possibly by using something like a large language model to parse the documentation rather than extracting HTML elements based on type and class name.

# Dependencies

- [Gumbo](https://github.com/google/gumbo-parser): An HTML5 parsing library in pure C99.
- [CURL](https://curl.haxx.se/libcurl/): A free and easy-to-use client-side URL transfer library.
- [Detours](https://github.com/microsoft/Detours): A software package for monitoring and instrumenting API calls on Windows.

I use vcpkg to download my packages. For visual studio I recommend running the `vcpkg integrate install` command.
