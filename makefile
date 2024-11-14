all:
	gcc ./src/*.c -I./include -o Mage-Game.out -lm

run:
	./Mage-Game.out

clean:
	rm Mage-Game.out