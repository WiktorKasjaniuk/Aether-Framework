### version 1.0.0
# Aether Framework
Aether Framework is a wrapper library around OpenGL and OpenAL functions programmed for designing 2D window applications and games.
My goal was to build a simple, yet polished framework, which would be a physical proof my programming skills and which would allow me to write low level applications.
The first version was designed in 200 days (I started working on it around 8th of March 2021), after that time I had finished all the neccessary parts and decided to publish it.
Currently, it contains a few singleton classes that manage the application, like AssetManager, LayerManager, EntityManager[ECS], Input and structures for both low and high level rendering.
The framework still has a lot of features to add, and unfortunately my time is limited.
Below is the list of planned functionalities.
### Download Builds
Aether Framework is written in `C++17` and for now, it has been compiled only to `Windows x32 & x64` and using `Microsoft Visual Studio Compiler`.
- [Aether Framework]()
- [Showcase Program]()
### Documentation
Due to the lack of time, I have not written the online documentation. Currently descriptions of all features are stored in .hpp files. If people start downloading and using this framework, I will make an online documentation, but this probably won't happen as there exist proffesional, well-documented libraries that counterpart the Aether Framework. In any case, if there are any questions or a need for brief or specific explanation, and I have the time, I will answer you.

# Brief List of Features
### Structure & System
- Vector2, Vector3, Vector4, Rectangle and Color structs for needed calcuations
- Time and Clocks
- Utf convertions
- Error logging system
- Application singleton to manage the project, Window
- AssetManager, SceneManager, LayerManager
- Input and Event dispatching through layers
- Cursor and Clipboard
- Custom Entity Component System with ability to view entities and components by: groups, component types and sets of included or excluded component types
### Graphics
- Matrix3x3, Transform2D, Camera for 2d transformations
- Font and a Text renderer with an option to retrieve char metrics
- Shader, Texture, VertexArray classes for rendering and higher level renderers, like Shape or Sprite
- Batch and instanced rendering abilities, also high level renderers
- Framebuffer and FrameSprite
- TextureCanvas for editing textures and images
- Stencil Testing
### Sounds
- Sound sources, Music class for real-time buffering
- Sounds and Music in 3D space, Listener

# "To Do" List
#### Project
- Compile framework to MacOS and Linux
- Write an online documentation
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
  - libflac is licensed under [this BSD-3-Clause license](https://github.com/xiph/flac/blob/master/COPYING.Xiph)
  - libogg is licensed under [this BSD-3-Clause license](https://github.com/xiph/ogg/blob/master/COPYING)
  - libvorbis is licensed under [this BSD-3-Clause license](https://github.com/xiph/vorbis/blob/master/COPYING)
- OpenAL Soft is licensed under [GNU Library General Public License v2](https://github.com/kcat/openal-soft/blob/master/COPYING)
- stb_image is licensed under [MIT License or Public Domain](https://github.com/nothings/stb/blob/master/LICENSE)
- utfcpp is licensed under [Boost Software License v1.0](https://github.com/nemtrif/utfcpp/blob/master/LICENSE)

# Set Up
First of all, download the library using the link from README.md file. The folder should contain `include` and `bin` directories.
Inside the bin folder, you will find all of the libraries needed for the framework to compile. Aether Framework itself comes with both Debug and Release builds.
### Project
- Create a new project in **Visual Studio** and open **Project Properties**
- **[Configuration Properties/General]** Set it's C++ standard to at least `ISO C++17`
- **[C/C++/General]** Add an additional include directory `Your\Directory\Aether Framework\include\`, so that the framework's `include` folder is there 
- **[Linker/General]** Add an additional library directory, where you will put all the `.lib` files 
- **[Linker/Input]** Add `AetherFramework-x32-Debug.lib;openal32.lib;libsndfile-1.lib;freetype.lib;glfw3.lib;opengl32.lib;kernel32.lib;user32.lib;gdi32.lib;` to additional dependencies, For win32 and 64, there are two debug builds: `AetherFramework-x32-Debug.lib` and `AetherFramework-x64-Debug.lib`, you can also use `AetherFramework-x$(PlatformArchitecture)-$(Configuration).lib` macro for all modes
- And for the last thing: remember to put the `.dll` files into your output directory, so that the `.exe` files are with them

### Simplest Project
```cpp

#include <Aether.hpp>

struct MyLayer : public ae::Layer {
	ae::Sprite sprite;

	MyLayer() {

		// set sprite's size (Sprite can detect it automaticly depending on the texture size, but it's always better to set it manually)
		sprite.SetSize(ae::Vector2f(500.f, 500.f));

		// retrieve a reference to the texture from AssetManager, and tell the sprite to use it
		ae::Texture& texture = ae::AssetManager.GetTexture("my_texture");
		sprite.SetTexture(texture);
	}

	virtual void Draw() override {
		sprite.Draw();
	}
	virtual void HandleEvent(ae::Event& event) override {

		// dispatch the event
		// if the event is EventContextClosed, close the application
		event.Dispatch<ae::EventContextClosed>(
			[](auto& event) -> bool {
				ae::Application.Close();

				// the event has been handled by this layer, no need to pass it to the next
				return true;
			}
		);
	}
};
struct MyScene : public ae::Scene {
	MyLayer* layer;

	MyScene() {

		// load assets to use them later
		ae::AssetManager.LoadTexture("my_texture", "my_texture.png");

		// create a new layer on top
		layer = &ae::LayerManager.CreateLayer<MyLayer>(true);
	}

	~MyScene() {

		// destroy scene's layer
		ae::LayerManager.RemoveLayer(*layer);
	}

	virtual void Update() override {

		// update the application
		// this function is called every frame after the input handling
		float some_normalized_value = sin(ae::Application.GetRunTime().GetSeconds()) / 2.f + 0.5f;

		ae::Color color = ae::Color::Black;
		color.g = 255 * some_normalized_value;

		layer->sprite.SetColor(color);
	}
};

int main()
{
	// intitialize the application and create a window of size (800, 600) and title "Aether Window!"
	ae::Application.Initialize(ae::Vector2f(800.f, 600.f), "Aether Window!");

	// register the scene
	ae::SceneId my_scene_id = ae::SceneManager.RegisterScene<MyScene>();

	// tell the framework, that you want to start(or change to) a new scene when possible
	ae::SceneManager.NextScene(my_scene_id);

	// run the application
	ae::Application.Run();
}
```
