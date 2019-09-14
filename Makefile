COMPILER = gcc
FLAGS = -c -O3

labirint: main.o labyrinthProcessing.o labyrinthBmpProcessing.o
	$(COMPILER) main.o labyrinthProcessing.o labyrinthBmpProcessing.o -o labirint

main.o: main.c labyrinthProcessing.h labyrinthBmpProcessing.h
	$(COMPILER) $(FLAGS) main.c

labyrinthProcessing.o: labyrinthProcessing.c labyrinthProcessing.h
	$(COMPILER) $(FLAGS) labyrinthProcessing.c

labyrinthBmpProcessing.o: labyrinthBmpProcessing.c labyrinthBmpProcessing.h
	$(COMPILER) $(FLAGS) labyrinthBmpProcessing.c


clean:
	rm -rf *.o labirint