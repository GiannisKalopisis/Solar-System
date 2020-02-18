# Solar-System
This project is a representation of the Sun-Earth-Moon system using the graphics library OpenGL in C++.

## Transformations and Models
For this project used 3 .obj files for the Sun, Earth and Moon respectively. All the transformations that are used to the objects have been written in reverse order in the code because of the multiplication property of the tables, but they are actually executed in the order they appear in the report. The transformations used in each object are:

- **Sun**

  - **Scale** to resize the Sun object properly
  - **Transpose** to move it to the center (0,0,0) of the axes or anywhere else we want 

- **Earth**
  
  - **Scale** to resize the Earth object properly
  - **Rotation** to make it rotate around its axis
  - **Transpose** to put it in the proper position / orbit around the Sun. This is done by changing the x and z coordinates of the Earth through the trigonometric functions *sin()* and *cos()* respectively and multiplying them by
the Earth's distance from the Sun.

- **Moon**

  - **Scale** to resize the Moon object properly
  - **Transpose** to make the Moon rotate around the center of the axes (orbit around the Sun) like Earth
  - **Transpose** to transfer the rotation of the Moon from the center of the axes (orbit around Sun), around the center of the Earth, through the trigonometric functions *sin()* and *cos()*.

## Lighting

For *lighting* a light source is placed in the center of the coordinates (0,0,0) into the Sun so the light is scattered everywhere in our scene. For the Sun there are two shaders, a vertex shader and a fragment shader, so that the Sun's object is not affected by the light. For Earth and Moon there are 2 other shaders, again a vertex shader and a fragment shader to influence objects by light. At the shaders of the Earth and the Moon calculated:

  - **Ambient Lighting**
  - **Diffuse Lighting**
  - **Specular Lighting**
  
and all combine through the **Phong model**. Lighting also uses the point light technique, as a light spot is a light source with a given location somewhere in our scene (0,0,0), which illuminates in all directions where the light rays fade with the distance, as with the rays of the Sun or a simple lamp in reality. All parameters for the Phong model or point light pass through the fragment shader through the code and can be easily changed.

## Camera

For the camera used the *camera class* from the *OpenGL tutorial* and the corresponding transformations were made to place the camera a little further away (to the positive z axis) from the scene and look at our objects with a view to avoid distortion and have strange visual effects.

## User Interface

To interact with the user we have the following functions:

  - **X-axis**: with the **A** and **D** keys the user can move his/her camera **left** and **right** on the X-axis.
  - **Y-axis**: with the **W** and **S** keys the user can move his/her camera **up** and **down** the Y-axis.
  - **Z-axis**: with the **↑** and **↓** keys or the mouse washer, the user can move his/her camera **forward** and **backward** on the Z-axis.
  - **Start/Pause**: with the **Space** key, the user can stop the rotation of the Earth and the Moon, and press it again to restart where they had stopped.
  - **Termination**: with the **Escape** key the user can exit the application.
  - **Mouse movement**: the user can use the mouse to change the angle the camera is looking at.

## Platform - Code

The work was developed in *CLion* in a *Linux* environment. The file CMakeLists.txt from the tutorial has been modified to fit it to work requirements. You can build the project from scratch by following the instructions in the [OpenGL repository](https://github.com/JoeyDeVries/LearnOpenGL) if you want to run it to another operating system of just rebuild it, or open the project with CLion (or another equivalent editor) and run it.
