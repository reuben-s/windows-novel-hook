# windows-novel-hook
Novel way to hook Windows API functions

# What is it?
A few months ago, I wanted to intercept all Windows API functions in a running executable for monitoring purposes. Initially, I thought it was impossible because Detours, the hooking library, requires function signatures at compile time.

However, I found a solution by inspecting the import table of a PE file and cross-referencing imported functions with the Windows documentation, which often includes function signatures. I extracted these signatures, compiled them into a DLL, and hooked the functions dynamically at runtime using Detours by loading the new DLL into the target.
