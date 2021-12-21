# RenderingEngine

## Background
This project was my subission for the year 3 Computer Graphics coursework. The task was to build a rendering engine and then use it to create a 20 second animated 'ident' to showcase its capabilities. The features of my rendering engine include:
* parse OBJ and MTL files
* wireframe, rasterised and ray-traced
* texture mapping (rasterised only)
* camera movement (translation and rotation)
* camera orientation (A.K.A panning)
* camera orbit
* ambient, proximity and angle of incidence lighting
* flat, gouraud and phong shading
* mirrored/reflective surfaces

## How to run

Make sure that `GLM` and `SDL2` are installed. Navigate to the project directory and run using

`make` for the default build;

`make diagnostic` for additional debugging output;

`make speedy` for a faster, optimised build; or

`make production` to compile and link for final release

When running, a SDL window should appear, displaying a rasterised render of the texture mapped Cornell Box model. The window's title contains information about the current toggled options for ray tracing (regardless of whether you are viewing in ray traced mode or not:

`SHDO(NONE | HARD | SOFT)` is the type of shadow being used

`PROX(0 | 1)` is whether proximity lighting is toggled

`INCI(0 | 1)` is whether angle of incidence lighting is toggled

`SPEC(0 | 1)` is whether specular highlighting is toggled

`AMBI(0 | 1)` is whether ambient lighting is toggled

`SHDE(FLAT | GOUR | PHON)` is the type of shading being used (flat, Gouraud or Phong)

Use the keypad numbers 1 - 4 to toggle between viewing options:

`1` for pointcloud viewing mode

`2` for wireframe viewing mode

`3` for rasterised viewing mode

`4` for ray traced viewing mode

When in rasterised or ray traced mode, press

`/` to switch between viewing all triangles or just the selected triangle

`TAB` to select the next triangle

`BACKSPACE` to select the previous triangle

Use the keypad numbers 5 - 9 and 0 to toggle between the ray traced options (see above):

`5` for shadow

`6` for proximity lighting

`7` for angle of incidence lighting

`8` for specular highlights

`9` for ambient lighting

`0` for shading

Use the arrow keys to change camera orientation, `QWEASD` keys to change camera position and `YUIHJK` keys to change light position.

