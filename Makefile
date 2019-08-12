COMPILER = gcc
FLAGS = -c -O3

labirint: main.o labyrinthProcessing.o 
	$(COMPILER) main.o labyrinthProcessing.o -o labirint

main.o: main.c bmpDescriptions.h labyrinthProcessing.h
	$(COMPILER) $(FLAGS) main.c

labyrinthProcessing.o: labyrinthProcessing.c labyrinthProcessing.h
	$(COMPILER) $(FLAGS) labyrinthProcessing.c

clean:
	rm -rf *.o labirint