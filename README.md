# cg-notes-code
Code examples and data for the [cg-notes repo](https://www.github.com/ibesora/cg-notes/)

Examples in this repo use the following libraries:
[Assimp](https://github.com/assimp/assimp) to import assets
[Bullet](https://github.com/bulletphysics/bullet3) to simulate physics
[Easy Profiler](https://github.com/yse/easy_profiler) as the CPU profiler
[Etc2Comp](https://github.com/google/etc2comp) to convert textures into the ETC2 format
[GLFW](https://github.com/glfw/glfw) to create the window
[GLI](https://github.com/g-truc/gli) to load and manipulate images
[GLM](https://github.com/g-truc/glm) as the math library
[GLSLang](https://github.com/KhronosGroup/glslang) to compile GLSL
[ImGui](https://github.com/ocornut/imgui) for a basic UI
[MeshOptimizer](https://github.com/zeux/meshoptimizer) to perform mesh operations
[Optick](https://github.com/bombomby/optick) as the GPU profiler
[STB](https://github.com/nothings/stb) to load and manipulate images
[Taskflow](https://github.com/taskflow/taskflow) to handle parallel programming
[Volk](https://github.com/zeux/volk) to load Vulkan endpoints
[Vulkan](https://github.com/KhronosGroup/Vulkan-Headers) as the rendering backend for some examples

## Examples
* **01_GLFW**: Creation of a GLFW window
* **02_Triangle**: Shows how to create, compile and link shaders into a program
* **03_Maths**: Uses GLM to compute a MVP matrix to show a rotating cube
* **04_SingleBuffer**: The same as before but using a double-sized buffer and __glBindBufferRange__ to draw each one instead of having to use multiple __glNamedBufferSubData__ calls

## Downloading dependencies
Just run `python bootstrap.py`

## Generating the visual studio solution
```
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
start CGNotesCode.sln
```
