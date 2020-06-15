# Code Optimization

Examples and tutorials for code performance improvements in C++

## Getting Started

### Prerequisites

You need to install [CMake](https://cmake.org/) to build the project, and a C++ compiler which handles C++17.

#### Linux

```bash
sudo apt-get install build-essential cmake libsdl2-dev libglu1-mesa-dev mesa-common-dev
```

#### Windows

To get the MSVC compiler, you need to install [Visual Studio](https://visualstudio.microsoft.com/) (and not VSCode), and select C++ development during installation.

### Build on desktop

You can handle the `CMakeLists.txt` in any way you like, here's some way to use it :

#### `Option 1: CLI`

Go to the folder of this project and run :

```bash
mkdir build
cd build
cmake ..
make
```

#### `Option 2: Visual Studio (Windows only)`

Open this folder with the `CMake...` option in file->open on Visual Studio, and run the project.

![Visual studio](doc/readme-img/visual-studio-run.png)

#### `Option 3: VSCode`

Use the `CMakeTools` plugin, build with `f7` then run with `f5` (But be carefull to be on the right platform, there is a launch file for windows and for linux).

![VS Code](doc/readme-img/vscode-run.png)

## TODO

Memory coherency when iterating List vs Array


