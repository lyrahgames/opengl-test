<h1 align="center">
    OpenGL Test
</h1>

<p align="center">
    Simple OpenGL Application Rendering a Colored Triangle to Test the Configuration of the Development Environment
</p>

<p align="center">
    <img src="https://img.shields.io/github/languages/top/lyrahgames/opengl-test.svg?style=for-the-badge">
    <img src="https://img.shields.io/github/languages/code-size/lyrahgames/opengl-test.svg?style=for-the-badge">
    <img src="https://img.shields.io/github/repo-size/lyrahgames/opengl-test.svg?style=for-the-badge">
    <a href="COPYING.md">
        <img src="https://img.shields.io/github/license/lyrahgames/opengl-test.svg?style=for-the-badge&color=blue">
    </a>
</p>

<p align="center">
    <img src="screenshot.png">
</p>

## Requirements

<b>
<table align="center">
    <tr>
        <td>Language Standard:</td>
        <td>C++17</td>
    </tr>
    <tr>
        <td>Operating System:</td>
        <td>Linux | Windows</td>
    </tr>
    <tr>
        <td>Compiler:</td>
        <td>GCC | Clang | Nuwen MinGW | MinGW | MSVC</td>
    </tr>
    <tr>
        <td>Build System:</td>
        <td>
            <a href="https://build2.org/">build2</a> | manually
        </td>
    </tr>
    <tr>
        <td>Dependencies:</td>
        <td>
            <a href="https://glbinding.org/">
                glbinding ^ 3.1.0
            </a>
            <br>
            <a href="https://www.glfw.org/">
                GLFW ^ 3.3.4
            </a>
            <br>
            <a href="https://github.com/g-truc/glm">
                GLM ^ 0.9.9.8
            </a>
        </td>
    </tr>
</table>
</b>

## Build System Explanation
This simple test consists mainly of the `buildfile` and the `main.cpp` file.
Typically, build2 demands a more sophisticated composition of buildfiles to allow for advanced functionality, like persistent configurations.
Because this is a simple test, using the simplest build mechanisms based on a single `buildfile` seemed to achieve the most comprehension.
Furthermore, in my opinion, it shows the superiority of the build2 build system compared to Make or CMake.

But for testing reasons and for enabling configuration in an editor, we could easily add further functionality by adding the `build/bootstrap.build` file with the following content.
```
# build/bootstrap.build
project = opengl-test

using config
using test
```
Now, run `b configure` to generate a valid default `build/config.build` file and adjust your in-source persistent configuration.
```
# build/config.build
# ...
config.cxx = #...
config.cxx.poptions = #...
config.cxx.coptions = #...
config.cxx.loptions = #...
# ...
```
Afterwards, running `b test` without all those command-line arguments to compile and run the application should suffice.

## Build and Run with build2
This code explicitly uses only the build system of the build2 compiler toolchain to elaborate on alternative build techniques.

### Linux
Make sure all dependencies are installed.
Open a terminal in the project root and run the following command to build and run the code.

    b test

For a custom configuration, you can run something like the following.

    b test \
        config.cxx=clang++ \
        "config.cxx.coptions=-O3 -march=native"

### Statically Cross-Compile on Linux for Windows
For 64-bit Windows platforms, you need to install the [Mingw-w64](https://www.mingw-w64.net) toolchain.
Also you have to make sure to satisfy all the required dependencies for this kind of compiler.
The best thing to do for now is compile statically.
Hence, you need to be able to access `libglbinding.a` and `libglfw3.a`.
A static variant of GLFW can be officially downloaded from [here](https://www.glfw.org/download.html).
For a static variant of `glbinding`, one has to manually compile it with a custom CMake toolchain file.
One also needs to put all headers from GLM, glbinding, and GLFW into the include directory.
Assuming all manually installed libraries can be found by using the prefix `/usr/local/x86_64-w64-mingw32`, the following command should create an executable `opengl-test.exe` runnable on Windows and by using Wine also on Linux (for this prepend test before the variable definitions).

    b \
        config.cxx=x86_64-w64-mingw32-g++ \
        config.cxx.poptions="-I/usr/local/x86_64-w64-mingw32/include" \
        config.cxx.coptions=-O3 \
        config.cxx.loptions="-L/usr/local/x86_64-w64-mingw32/lib -fPIC -static -static-libgcc -static-libstdc++" \
        config.cxx.libs="-lgdi32"

### Windows
#### Nuwen MinGW Compiler Distribution
On Windows, I recommend to use the [Nuwen MinGW compiler distribution](https://nuwen.net/mingw.html) because in this distribution of MinGW all needed dependencies are already installed.
Use a custom configuration to be able to find the static libraries.
We want to use the static version of the GLFW library.
Hence, we have to additionally link the library `gdi32` into the application.

    b test \
        config.cxx=g++ \
        "config.cxx.poptions=-IC:/MinGW/include" \
        "config.cxx.coptions=-O3 -march=native" \
        "config.cxx.loptions=-LC:/MinGW/lib" \
        "config.cxx.libs=-lgdi32"

#### Microsoft C++ Compiler or MinGW
Using the Microsoft C++ compiler or the standard MinGW compiler distribution, we will probably use dynamic libraries.
So make sure to install all dependencies to appropriate locations and get their include and library paths.
Of course, we have to customize this configuration as well.
Call the following command inside the developer prompt of the MSVC.

    b test \
        config.cxx=cl \
        "config.cxx.poptions=/IC:/GLFW/include /IC:/glbinding/include /IC:/glm" \
        "config.cxx.coptions=/O2" \
        "config.cxx.loptions=/LIBPATH:C:/GLFW/lib /LIBPATH:C:/glbinding/lib"

## Usage
- Escape: Quit the program.

## Additional Information
- [Authors](AUTHORS.md)
- [License](COPYING.md)

## References
- [Learn OpenGL](https://learnopengl.com/Getting-started/OpenGL)
- [opengl-tutorial](https://www.opengl-tutorial.org/)
- [build2 Toolchain Introduction](https://build2.org/build2-toolchain/doc/build2-toolchain-intro.xhtml)
- [build2 Build System Manual](https://build2.org/build2/doc/build2-build-system-manual.xhtml)
- [nuwen.net MinGW Distro](https://nuwen.net/mingw.html)
- [glbinding](https://glbinding.org/)
- [GLFW](https://www.glfw.org/)
- [GLM](https://glm.g-truc.net/0.9.9/index.html)
- [Khronos OpenGL Reference Pages](https://www.khronos.org/registry/OpenGL-Refpages/)
- [GLSL Language Specification](https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.4.50.pdf)
