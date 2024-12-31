COMPILER = clang++
SOURCE_INC = -Ilib/ -Ilib/Elements/ -I/opt/homebrew/Cellar/sfml/2.6.1/include
SOURCE_LIBS = -Llib/ -Llib/Elements/ -L/opt/homebrew/Cellar/sfml/2.6.1/lib
OSX_OPT = -std=c++17 -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -lsfml-graphics -lsfml-window -lsfml-system
DEBUG = -g
OUTPUT_FILE = bin/build_osx
CFILES = src/*.cpp src/Elements/*.cpp

build_osx:
	$(COMPILER) $(CFILES) $(SOURCE_INC) $(SOURCE_LIBS) -o "$(OUTPUT_FILE)" $(OSX_OPT)
build_debug:
	$(COMPILER) $(CFILES) $(SOURCE_INC) $(SOURCE_LIBS) -o "$(OUTPUT_FILE)" $(DEBUG) $(OSX_OPT)
run:
	./$(OUTPUT_FILE)