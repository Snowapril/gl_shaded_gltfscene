# gl_shaded_gltfscene

[![CodeFactor](https://www.codefactor.io/repository/github/snowapril/gl_shaded_gltfscene/badge)](https://www.codefactor.io/repository/github/snowapril/gl_shaded_gltfscene)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/82c7218998df4b4eb236ad21ae96c1fe)](https://www.codacy.com/gh/Snowapril/gl_shaded_gltfscene/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Snowapril/gl_shaded_gltfscene&amp;utm_campaign=Badge_Grade)
![Ubuntu github action](https://github.com/Snowapril/gl_shaded_gltfscene/actions/workflows/ubuntu.yml/badge.svg?branch=main)
![Window github action](https://github.com/Snowapril/gl_shaded_gltfscene/actions/workflows/window.yml/badge.svg?branch=main)
![MacOS github action](https://github.com/Snowapril/gl_shaded_gltfscene/actions/workflows/macos.yml/badge.svg?branch=main)

This project visualizes gltf scene file with PBR shading techniques. The default lighting is Image-Based-Lighting which precomputes irradiance map and prefiltered glossy map and use them in the PBR shader. Material attributes passing implemented with simple structural uniform variables which can make performance low (This can be improved by ssbo). 

## Build
```bash
git submodule init
git submodule update
mkdir build
cd build
cmake .. && make
```

## Run
```bash
# in the generated executable file directory
gl_shaded_gltfscene -s "what_you_want.gltf" -e "hdr_environment.hdr"
```
Of course, without providing scene and environment files, rendering can be done with default resources.

## Screenshot
![image](https://user-images.githubusercontent.com/24654975/119116611-5d9c8700-ba63-11eb-8ecd-1682741b70bc.png)

## dependency
*   cxxopts
*   glad
*   glfw
*   glm
*   imgui
*   stb_image
*   tinyobjloader
*   tinygltf
*   draco

## License
<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

The class is licensed under the [MIT License](http://opensource.org/licenses/MIT):

gl_shaded_gltfscene project's scene and skydome are largely referenced on [vk_shaded_gltfscene](https://github.com/nvpro-samples/vk_shaded_gltfscene)

Copyright (c) 2021 Snowapril
*   [Jihong Shin](https://github.com/Snowapril)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
