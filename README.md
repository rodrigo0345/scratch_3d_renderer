# Cool Rendering Engine

Welcome to the Cool Rendering Engine! This project was built from scratch and uses a simple C array as its core rasterization buffer. It features real-time 3D rendering with basic lighting, shading, and object loading.

## Installation

To get started with the Cool Rendering Engine, you'll need to install `SDL2` which is used for painting the pixels on the screen and nothing more.

### Installing SDL2

#### On Ubuntu/Debian:

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
make
```

## Running the project
```sh
make run
```

## Commands
While the project is running, you can use the following commands:

- 2 - Draw lines only
- 3 - Fill triangles
- 4 - Fill and draw lines
- t - Load the tank model
- f - Load the F-22 model
- k - Load the cube model

## Additional Details

- The engine uses a simple C array for rasterization, making it lightweight and efficient.
- It currently supports basic 3D object loading and rendering with options for wireframe, filled triangles, and combined modes.

## Sources

https://courses.pikuma.com/

## Demo

Here are some examples of what the engine can do:

![](https://github.com/rodrigo0345/c_renderer/blob/main/assets/f22.mov)
![f22-shading](https://github.com/rodrigo0345/c_renderer/blob/main/assets/f22-shading.png)
![tank](https://github.com/rodrigo0345/c_renderer/blob/main/assets/tank.png)
