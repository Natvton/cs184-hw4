CC = g++
LDFLAGS =  -lglfw -lglut -lGLU -lGL -lX11

RM = /bin/rm -f 
all: 
	$(CC) $(CFLAGS) -o space main.cpp $(LDFLAGS) 
clean: 
	$(RM) *.o space
