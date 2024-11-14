all:
	gcc ./src/*.c -I./include -o Skyfall-Blitz.out -lm

run:
	./Skyfall-Blitz.out

clean:
	rm Skyfall-Blitz.out