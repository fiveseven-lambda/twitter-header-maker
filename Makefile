out.png: main.cpp
	g++ main.cpp -Wall -Wextra
	./a.out
	gm convert out.bmp out.png

clean:
	rm *.out *.bmp *.png
