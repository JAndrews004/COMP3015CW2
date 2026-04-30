# COMP3013CW2 - Jonathan Andrews

## Overview
This project is a scene meant to showcase different graphics techniques built off a previous project. The texhniques shown in this project are shadow mapping, wire frames using the geometry shader and a particle fountain. This project also showcases a simple puzzle for the player to solve using buttons in the scene while find in clues implemented in the previous version. This puzzle system uses a custom game state manager and button structures with some simple UI.

---

## Development Environment
IDE: Visual studio 2022
Operating System: Windows 11 Pro
OpenGL Version: 4.6
Dependencies/Libraries Used: GLFW, GLAD, GLM stb_image, freetype

## Instructions
To run the executable find it in /Executable and run the Project_Template.exe. All controls are below.

## Controls & User Interaction

| Action                                      | Key |
|---------------------------------------------|-----|
| Camera lateral movement     | WASD |
| Look around     | Mouse |
| Wireframe toggle| Q |
| Character interaction     | E |
| Fog toggle        | 0 |
| Play/ pause animation of light     | Space bar |
| Character 'torch' to find graffiti on statue.     | F |
|Exit program | ESC key|

When the user interacts with the WASD to move the cameras front vector is changed in the direction of movement by adding to the x-z values. This moves the view when it is recalculated in the render loop. The mouse movement changes the pitch and yaw of the camera which is then used to recalculate the front vector of the camera. When toggling a wireframe a boolean value is switched which changes which path the render function takes. This changes the shaders used to use the geometry shader and a simplified fragment shader as no lighting calculations are required. The player interaction is only active when they are within a buttons range, when this happens a UI prompt is shown to let the player know they are in range. When it is pressed it toggles its associated light. 0 toggles the fog by changing the value passed into the enabled uniform of the Fog structure in the fragment shader. Pressing F toggles the players 'torch' which is a spotlight attached to the camera facing the same way where there are 2 versions. One version is the physical light on the objects that the player can see this is toggled by changing the lights L value. The other version is used in the graffiti shader to show the texture of the graffiti to the player if it is inside the spot light cone. 

---

## System Architecture
### Program Structure
The program is organsised with the scenebasic_uniform.cpp holding the render function and any of the auxilary functions used for player input. This has references to the Button structures used for the player interactale buttons in the scene and a game state manager to manage the puzzle aspect of the scene. Each shader program is loaded in the compile function where error checking was added.

### Main Components
The renderer is located in scenebasic_uniform.render() and holds all the rendering logic for this project. In this the different model's matrices are edited to place the object and to allow movement with the camera and WASD. The renderer is split into 2 main logical blocks, one is for the normal rendering with lighting and textures, the other is for the wireframe rendering. Outside of these 2 blocks is the text UI logic with the button proximity and the game manager checking the puzzle state. The particle fountain logic is also located here. 


The main shaders added to this version of the project are shaders needed for: wireframes, shadow mapping, UI, and particle fountain. The wireframe's shaders consist of a fragment, vertex, and geometry shader. Each of the rest only use a fragment and vertex shader.
The input handling occures in the scenerunner.h.mainLoop this is where all the button presses are checked for and passed over to the scenebaseic_uniform class.
Object management occures in the scenebasic_uniform constructor. Here the objects are loaded from their files and the buttons created. The game manager handles the scene state with checking if the puzzle has been solved.


### Data Flow
The data that is calculated in the CPU is passed to the GPU and the shaders by using uniforms. This includes material properties, light properties, fog properties, textures etc.

The render loop goes through a basic process for each object. First the M,V,P are calculated for each and passed to the shaders. Next the material properties are passed over and then the textures are bound and finally the object is rendered. Before any of the objects are rendered however the shadow mapping pass is completed. This is done to get a depth map which will be used later in the loop. At the end of the loop the UI is rendered with the particle fountain if the puzzle is solved.

---

## Shader Implementation

### Base Shader Pipeline

The base shader pipline is used to render the main objects of the scene. The vertex shader calculates the TBN matrices for normal mapping and passes them to the fragment shader. It also calculates the normal and position and passes them with the texture coordinate to the fragment shader. 
In the fragment shader the lighting is calculated using a BlinnPhong model for each light. This is then gamma corrected before being outputted. Before the lighting is calculated the shadow is calculated for the spotlight that the shadow map was generated from. This is done in a seperate function which returns a value that will be mixed with the lighting from the spot light. The fog calulations are also done after the lighting and mixed depending on the distance.


### Shadow Mapping Implementation

For the shadow mapping implementation a depth map is generated at the start of the render loop. Before the depth map is generated the light space matrix is calculated and passed to the base fragment shader to be used in the shadow calculations. The program is then switched to a depth program. This consists of a fragment and vertex shader. The vertex shader calculates the gl_Position relative to the light. The shadow map is rendered to the frame buffer object and the depth buffer is cleared before the scene is rendered to the frame buffer. Then the frame buffer object recieves the depth values from the fragment shader. This is then saved as a texture to be used in the lighting calculations. The depth map allows the program to know which fragments are occluded by others and should be in shadow.
The shadow calculation takes the fragments position in world space coordinates and converts them to light space. Then it is compared to the depth map to see if it is occluded. Finally pcf filtering is applied to give it a softer edge and then the shadow value is returned to be used in the light calculations.

### Particle Rendering Shader
The particle fountain is implemented entirely on the GPU using a vertex shader that computed each particles position over time based on its initial velocity, gravity and lifetime. Each particle is rendered as a camera facing quad allowing efficient rendering of many particles with minimal CPU overhead. The program is set to render 80000 particles with perfance tanks. Each particle is rendered as a camera-facing billboard using vertex offsets, and transparency is interpolated over time to create smooth fading. This approach enables efficient rendering of dynamic particle effects while maintaining performance.

### Wireframe geometry shader
The wireframe is rendered using a geometry shader that calculates the distance of each fragment to the triangle edges. This allows edges to be highlighted smoothly in the fragment shader. A geometry shader is used to process each triangle individually. This would not be possible in the vertex shader alone. It is used to compute edge distance information for each vertex. The vertex positions are transformed into screen space using a viewport transformation. The triangle geometry is used to calculate the perpedicular distance to its opposite edge. These distances are later interpolated across triangles. The noperspective qualifier is used to ensure linear interpolation in screen space to prevent distortion of the edge thickness. In the fragment shader the interpolated edge distances are used to determin proximity to triangle edges. Fragments close to an edge are coloured differently producing the wireframe.

### Shader Enhancements
The main addition to the base shader program is the addition of the shadow mapping calculation and the gamma correction is fixed. No other shaders from the initial project were edited only new shaders were added. These are: flat_frag/vert.glsl, particleFountain.frag/vert, shadow.frag/vert, text.frag/vert, and wireframe.frag/gs/vert.
---

## Evaluation

### Use of AI
The main use I had of generative AI for this project was to help plan what order to implement features to gain the maximum marks 
early in case I ran out of time.

![Use of AI 1](../ScreenShots/1.png)

I also used it to help when i ran into unkown errors such as a LINK2019 error to save time in the fixing process. To do this I pasted the error message and asked how to fix it and it gave me a quick guide that worked and now I know what the error is and how to fix it in the future. 

![Use of AI 2](../ScreenShots/02.png)

Finally I used it to help me implement the UI elements as I have no experience with freetype or how to implement it into my project. There were also a lot of integration issues and issues with trying to get it to show the correct text properly.

![Use of AI 3](../ScreenShots/3.png)

### What Went Well
Overall, I believe my implementations of techniques into my project were executed well. This is becuse it runs smoothly with no crashes and works reliably. I also think the implementation of the puzzle and game state manager was done in a way that supported the architecture and expanded the project. 

### Limitations
In the intial project the spotlight moved in the scene. However to add visually pleasing shadow mapping i had to make it static as the shadow didnt move correctly with the light as the shadow map is generated using an orthopedic view. The shadows are also slightly too low quality and the edges are slightly blocky. The particles in my scene have no collision handling so they just go through the floor. Some more advanced animations such as wind could also improve the aesthetics of this. The use of a geometry shader introduces additional overhead, which can impact performance compared to simpler rendering approaches. The puzzle system is relatively simple and does not scale to more complex or dynamic scenarios.

### Future Improvements
In the future, improvements to the shadow mapping could improve the aesthetics of the scene dramatically. If the point lights had shadow mapping with cube maps more objects in the scene could have shadows not just the central statue. Improving the shadow quality would also help significantly possibly through anti-aliassing. Making the puzzle system more advanced with a submission function and lives would make it more dynamic and prevent players from brute forcing it. 

---

## Video Demonstration
[Link to video report](https://youtu.be/15YAJJIWhxE)

---

## References
Statue model: https://www.fab.com/listings/9c44c778-a88e-4060-8024-af45f0b8a7eb 
Statue main texture: https://www.freepik.com/free-photo/abstract-smooth-brown-wall-background-layout-designstudioroomweb-templatebusiness-report-with-smooth-circle-gradient-color_29885834.htm#fromView=keyword&page=2&position=17&uuid=c48f0b52-f3f6-43d6-a68a-7e69ace0799e&query=Bronze+texture 
Statue normal map: https://opengameart.org/node/21133 
Floor plane normal map: https://opengameart.org/node/21129 
Floor plane main texture: https://architextures.org/textures/583 
Floor plane moss mix: https://www.pngegg.com/en/png-mcngd/download 
Floor plane puddle mask: https://opengameart.org/content/noise-texture-pack-super-noise-8-512x512png 
Statue arrow graffiti: https://www.hiclipart.com/free-transparent-background-png-clipart-idybq 
Skybox: https://polyhaven.com/a/shanghai_bund 
Skybox to cubemap converter: https://hdri-to-cubemap-converter.vercel.app/

Plinth model:
Button model:
Wall texture: 
Wall normal map: 
