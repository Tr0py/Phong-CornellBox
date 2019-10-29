all:
	g++ ./main.c ./glad.c -lglfw -lGLEW -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lXcursor -o main -g
clean:
	rm ./main
