# Demonia

An OpenGL demo.

## Tested Platforms

- Arch Linux x86_64
    - Kernel version 5.18.6, Mesa OpenGL backend, Wayland compositor

## Requirements

- OpenGL version 3.3-compatible renderer

## Building

Shell commands to be run as user are denoted by `$`.

### Requirements

- C++11-compatible compiler with standard library (`gcc` >= 4.8.1)
- `cmake` >= 3.10
- `glew` >= 3.3
- `glfw` >= 3.3

### Release

`$ (mkdir -p build && cd build && cmake .. && make)`

### Debug

`$ (mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make)`

### Usage

`$ ./build/demonia`

## License

Copyright (C) 2022 Natalie Wiggins

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

See `LICENSE` for more information.
