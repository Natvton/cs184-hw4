CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -DOSX
	INCFLAGS = -I/usr/X11/include -I./include/
	LDFLAGS = -lglfw -framework Cocoa -framework IOKit -framework GLUT -L./osxlib/ -L"/System/Library/Frameworks/OpenGL.framework/Libraries" -lGL -lGLU -lm -lstdc++ -lfreeimage
else
	LDFLAGS = -lglfw -lglut -lGLU -lGL -lX11 -lfreeimage
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES
endif
RM = /bin/rm -f 
all: 
	$(CC) $(CFLAGS) -o space main.cpp camera.cpp keyboard.cpp glm.cpp shaders.cpp $(INCFLAGS) $(LDFLAGS) 
clean: 
	$(RM) *.o space
