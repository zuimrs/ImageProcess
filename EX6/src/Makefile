all:main.cpp CImg.h
	rm -f main.out
	g++ -o main.out main.cpp stitch.cpp ./siftpp/sift.cpp -g -I ./ -I ./siftpp -L/usr/X11R6/lib -lm -lpthread -lX11
