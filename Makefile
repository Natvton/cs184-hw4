CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	LDFLAGS = -lglfw -framework Cocoa -framework IOKit -framework GLUT -L"/System/Library/Frameworks/OpenGL.framework/Libraries" -lGL -lGLU -lm -lstdc++ -lfreeimage
else
	LDFLAGS = -lglfw -lglut -lGLU -lGL -lX11 -lfreeimage
endif
RM = /bin/rm -f 
all: 
	$(CC) -o space main.cpp camera.cpp keyboard.cpp glm.cpp $(LDFLAGS) 
clean: 
	$(RM) *.o space
