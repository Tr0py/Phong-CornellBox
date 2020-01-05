all:
	g++ ./main.cpp ./objloader.cpp ./objloader.hpp ./glad.c -lglfw -lGLEW -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lXcursor -o main -g stb_image.h -lassimp
clean:
	rm ./main
