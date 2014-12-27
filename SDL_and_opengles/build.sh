export CXXFLAGS=$(sdl2-config --cflags) && export LDLIBS="$(sdl2-config --libs) -lGL" && make SDL_and_opengles && ./SDL_and_opengles
