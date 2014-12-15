export CXXFLAGS=$(sdl2-config --cflags) && export LDLIBS="$(sdl2-config --libs) -lGL -lGLEW" && make 51_SDL_and_modern_opengl && ./51_SDL_and_modern_opengl
