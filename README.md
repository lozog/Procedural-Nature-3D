# 3D Nature Environment with Procedurally Generated Trees

---

## Dependencies
* OpenGL 3.2+
* GLFW
    * http://www.glfw.org/
* Lua
    * http://www.lua.org/
* Premake4
    * https://github.com/premake/premake-4.x/wiki
    * http://premake.github.io/download.html
* GLM
    * http://glm.g-truc.net/0.9.7/index.html
* ImGui
    * https://github.com/ocornut/imgui
* Simple OpenGL Image Library
    * http://www.lonesock.net/soil.html

---

## Build
As with other CS488 projects, this program uses **premake4** as the cross-platform build system. First you will need to build all
the static libraries that the projects depend on. To build the libraries, open up a
terminal, and **cd** to the top level of the CS488 project directory and then run the
following:

    $ premake4 gmake
    $ make

This will build the following static libraries, and place them in the top level **lib**
folder of your cs488 project directory.
* libcs488-framework.a
* libglfw3.a
* libimgui.a
* libSOIL.a

Next we can build the actual project.  To do this, **cd** into the A5/ directory and run the following terminal commands in order to compile the A5 executable using all .cpp files in the A5 directory:

    $ cd A5/
    $ premake4 gmake
    $ make

----

## Other Resources

* grass.png
    * copyright Blender Foundation | apricot.blender.org

* water.png
    * retrieved from http://opengameart.org/content/3-live-proceduraly-generated-tiling-water-textures-512px-running-brushes

* skybox textures
    * created by Zachery "skiingpenguins" Slocum
    * retrieved from http://opengameart.org/content/skiingpenguins-skybox-pack

* bark.png
    * created by Bart. K
    * retrieved from http://opengameart.org/content/seamless-tiling-tree-bark-texture

* sgrass5-1.png, sgrass5-3.png
    * created by virtushda
    * retrieved from http://opengameart.org/content/grass-2-0

* leaf.png
    * retrieved from http://www.textures.com/download/leaves0120/27993