all: Mandelbrot.o
	g++ Mandelbrot.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system -lpthread
	rm *.o
Mandelbrot.o: Mandelbrot.cpp
	g++ -c Mandelbrot.cpp -g
clean:
	rm -f *.o
	rm sfml-app
	
