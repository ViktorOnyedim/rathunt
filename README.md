# Rathunt

## Introduction
This project is a C++ implementation of a raycasting engine, inspired by the classic game Wolfenstein 3D. The engine features 3D rendering of walls, a first-person perspective, and the ability to add sprite-based enemies to the game world.  
The final project can be accessed at https://github.com/ViktorOnyedim/rathunt and a detailed blog article about the development process is available at https://www.linkedin.com/in/viktor-onyedim. The project was created by Victor Onyedim and you can connect with the author on https://www.linkedin.com/in/viktor-onyedim.

## Installation
To run the raycasting engine, you'll need to have the following dependencies installed:

- SDL2
- SDL2_image  

You can install these dependencies using your system's package manager. For example, on Ubuntu, you can run:  
`sudo apt-get install libsdl2-dev libsdl2-image-dev`
  
Once the dependencies are installed, you can clone the repository and build the project:  
`git clone https://github.com/ViktorOnyedim/rathunt.git  
cd rathunt  
g++ rathunt.cpp -w -lSDL2 -lSDL2_image -o rathunt`  


## Usage
To run the raycasting engine, execute the following command

`./rathunt` 

The game will start, and you can use the following controls:

- W: Move forward
- S: Move backward
- A: Strafe left
- D: Strafe right
- Left Arrow: Rotate left
- Right Arrow: Rotate right

The game includes sprite-based enemies that you can see and interact with in the 3D environment.
Contributing
If you'd like to contribute to this project, please follow these steps:

1. Fork the repository
2. Create a new branch for your feature or bug fix
3. Make your changes and test them thoroughly
4. Submit a pull request with a detailed description of your changes  

## Screenshot
![screenshot](https://github.com/ViktorOnyedim/rathunt/assets/screenshot.png)

## Related Projects

- Wolfenstein 3D - The classic first-person shooter that inspired this project

## Licensing
This project is licensed under the MIT License.




