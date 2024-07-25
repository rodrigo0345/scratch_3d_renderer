![banner](https://github.com/rodrigo0345/scratch_3d_renderer/blob/main/assets/highlight.png)

# Cool Rendering Engine

Welcome to the Cool Rendering Engine! This project was built from scratch and uses a simple C array as its core rasterization buffer. It features real-time 3D rendering with basic lighting, shading, and object loading.
Some of the assets I used are exclusive to Gustavo Pezzi and therefore, they were not included in this repository.
Credits to https://courses.pikuma.com/ for making computer graphics just the most fun stuff ever.

## Installation

To get started with the Cool Rendering Engine, you'll need to install `SDL2` which is used for painting the pixels on the screen and nothing more.


### Installing SDL2

#### On Ubuntu/Debian:

```sh
sudo apt-get update
sudo apt-get install cmake make
```

```sh
sudo apt-get update
sudo apt-get install libsdl2-dev
```

On MacOS using Homebrew:
```sh
brew install sdl2
```

## Building the project

Once you have SDL2 installed, you can build the project using `make`.
```sh
cmake -S . -B build
```

*or* using `run.sh` you can run the project directly.

```sh
chmod +x ./run.sh; ./run.sh
```


## Running the project

```sh
cd build; 
make; 
cd ..; 
./build/rendering_engine
```

## Debugging the project

```sh
cd build; 
make; 
cd ..; 
gdb ./build/rendering_engine
```

## Commands
While the project is running, you can use the following commands:


### General Controls

- Escape (ESC): Exit the application.
- B (b): Toggle windowed mode.
- 0 (0): Toggle classic mode.
- 1 (1): Switch to Wireframe with small dots display mode.
- 2 (2): Switch to Wireframe lines display mode.
- 3 (3): Switch to Solid display mode.
- 4 (4): Switch to Solid and Wireframe display mode.
- 5 (5): Switch to Textured display mode.
- 6 (6): Switch to Textured and Wireframe display mode.

### Camera Controls

- W (w): Move the camera forward.
- S (s): Move the camera backward.
- A (a): Yaw the camera to the left.
- D (d): Yaw the camera to the right.
- Left Shift (LSHIFT): Move the camera upward.
- Left Control (LCTRL): Move the camera downward.
- Up Arrow (UP): Pitch the camera upward.
- Down Arrow (DOWN): Pitch the camera downward.

### Optimization Controls

This optimization uses subpixel arrangement as well as a clever
way to calculate the barycentric coordenates.

- P (p): Toggle optimized rendering mode.

## Additional Details

- The engine uses a simple C array for rasterization, making it lightweight and efficient.
- It currently supports basic 3D object loading and rendering with options for wireframe, filled triangles, and combined modes.

## Demo

Here are some examples of what the engine can do:

[![Demo F22](https://github.com/rodrigo0345/scratch_3d_renderer/blob/main/assets/f22-shading.png)](https://github.com/rodrigo0345/c_renderer/blob/main/assets/f22.mov)
![tank](https://github.com/rodrigo0345/scratch_3d_renderer/blob/main/assets/tank.png)
