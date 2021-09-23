### version 1.0.0
# Aether Framework
Aether Framework is a wrapper library around OpenGL and OpenAL functions programmed for designing 2D window applications and games.
My goal was to build a simple, yet polished framework, which would be a physical proof my programming skills and which would allow me to write low level applications.
The first version was designed in 200 days (I started working on it around 8th of March 2021), after that time I had finished all the neccessary parts and decided to publish it.
Currently, it contains a few singleton classes that manage the application, like AssetManager, LayerManager, EntityManager[ECS], Input and structures for both low and high level rendering.
The framework still has a lot of features to add, and unfortunately my time is limited.
Below is the list of planned functionalities.

# "To Do" List
#### Project
- Compile framework to MacOS and Linux
#### Utitilty
- Move and copy operators' overloads for Assets
- Option to choose between quick sort and insertion sort while sorting EntityComponentSystem views
- String class instead of ae::utf namespace, which would be stored in UTF-16 format and convert between UTF-8, UTF-16, UTF-32
- Datetime and Calendar classes
#### Graphics
- 1D, 2D, 3D textures with 1, 2, 3 or 4 channels instead of a fixed 2D, 4 channel one
- Loading multiple faces to one font instead of just one
- 3D upgrade: Matrix4x4, Transform3D, 3D Camera, Mesh, Depthbuffer
- Framebuffer antialiasing
- Converting stencil buffer to TextureCanvas
- Explicitly defined default Framebuffer (DefaultFramebuffer.Bind() instead of framebuffer.Unbind())
#### Interface
- Multiple windows
- GUI - due to the lack of time and free, quality alternatives, I have decided to not implement a GUI in the first version
#### Sounds:
- Capture devices and recording sounds
- Checking for deafault device if it was not present at the beginning of application, 
and ability to connect it while application is running

# Alternatives
There are obviously many other, serious projects, alternatives to Aether Framework, including:
- [SFML (Simple and Fast Multimedia Library)](https://www.sfml-dev.org/)
- [SDL (Simple Directmedia Layer)](https://www.libsdl.org/)

# Used Libraries
All the licenses can be found here or inside the reposition.
- FreeType is licensed under [FreeType License](https://gitlab.freedesktop.org/freetype/freetype/-/blob/master/docs/FTL.TXT) or [GNU General Public License v2](https://gitlab.freedesktop.org/freetype/freetype/-/blob/master/docs/GPLv2.TXT)
- glad is licensed under [MIT License](https://github.com/Dav1dde/glad/blob/master/LICENSE)
- glfw is licensed under [zlib/libpng](https://www.glfw.org/license)
- libsndfile is licensed under [GNU Lesser General Public License v2.1](https://github.com/libsndfile/libsndfile/blob/master/COPYING)
  - libflac is licensed under [this license](https://github.com/xiph/flac/blob/master/COPYING.Xiph)
  - libogg is licensed under [this license](https://github.com/xiph/ogg/blob/master/COPYING)
  - libvorbis is licensed under [this license](https://github.com/xiph/vorbis/blob/master/COPYING)
- OpenAL Soft is licensed under [GNU LIBRARY GENERAL PUBLIC LICENSE v2](https://github.com/kcat/openal-soft/blob/master/COPYING)
- stb_image is licensed under [MIT License or Public Domain](https://github.com/nothings/stb/blob/master/LICENSE)
- utfcpp is licensed under [Boost Software License v1.0](https://github.com/nemtrif/utfcpp/blob/master/LICENSE)
