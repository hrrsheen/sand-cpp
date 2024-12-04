COMPILER = clang++
SOURCE_INC = -Ilib/ -Ilib/elements/ -I/opt/homebrew/Cellar/sfml/2.6.1/include
SOURCE_LIBS = -Llib/ -Llib/elements/ -L/opt/homebrew/Cellar/sfml/2.6.1/lib
OSX_OPT = -std=c++17 -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -lsfml-graphics -lsfml-window -lsfml-system
DEBUG = -g
OSX_OUT = -o "bin/build_osx"
CFILES = src/*.cpp src/elements/*.cpp

build_osx:
	$(COMPILER) $(CFILES) $(SOURCE_INC) $(SOURCE_LIBS) $(OSX_OUT) $(OSX_OPT)
build_debug:
	$(COMPILER) $(CFILES) $(SOURCE_INC) $(SOURCE_LIBS) $(OSX_OUT) $(DEBUG) $(OSX_OPT)