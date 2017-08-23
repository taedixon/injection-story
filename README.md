# Injection Story


This project provides an alternative approach to Cave Story modding/code injection using DLL hijacking through a crafted `version.dll` file. The main advantage of this approach is that it requires no direct modifications to the executable itself in order to perform the initial code injection. However, without installing hooks into the game exe beforehand the number of things you can actually do with an unmodified exe are fairly limited.

The original purpose of this was to circumvent certain rules about exe editing in a cave story modding competition. However, by extending the concept I've found it fairly convenient to write "major hacks" this way rather than directly in asm.

As I'm attempting to keep my modding activities Open, any projects I work on using this framework will be included as a subdirectory unless at some point I change my mind or figure out a better way to organize it.
