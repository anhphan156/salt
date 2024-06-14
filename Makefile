all: main.o
	gcc -o out main.o -lm

main.o: main.c
	gcc -g -c main.c -o main.o

clean:
	rm *.o out

run:
	rm *.o
	make
	./out
